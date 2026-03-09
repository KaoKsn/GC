#include "vm.h"
#include <stdio.h>

vm_t *create_vm()
{
    vm_t *vm = malloc(sizeof(vm_t));
    if (vm == NULL) {
        fprintf(stderr, "Mem Allocation for VM Failed!\n");
        return NULL;
    }
    // Create initialize the frames and objects stack.
    vm->frames = get_stack(8);
    if (vm->frames == NULL) {
        free(vm);
        fprintf(stderr, "Failed to initialize a stack frame!\n");
      return NULL;
    }
    vm->objects = get_stack(8);
    if (vm->objects == NULL) {
        free_stack(vm->frames);
        free(vm);
        fprintf(stderr, "Failed to initialize an object frame!\n");
        return NULL;
    }
    return vm;
}

void free_vm(vm_t *vm)
{
    if (vm == NULL) {
        return;
    }
    // Free all the frames in the VM.
    for (size_t i = 0; i < vm->frames->top; i++) {
        // Free every frame.
        free_frame(vm->frames->data[i]);
    }
    free_stack(vm->frames);
    // Free all the objects associated to the VM.
    for (size_t i = 0; i < vm->objects->top; i++) {
        free_obj(vm->objects->data[i]);
    }
    free_stack(vm->objects);
    free(vm);
}

// Create a frame, and push it to the frames stack in VM.
frame_t *get_frame(vm_t *vm)
{
    if (vm == NULL) {
        fprintf(stderr, "VM = NULL!\n");
        return NULL;
    }
    frame_t *frame = malloc(sizeof(frame_t));
    if (frame == NULL) {
        fprintf(stderr, "Failed creating a frame!\n");
        return NULL;
    }
    frame->references = get_stack(8);
    if (frame->references == NULL) {
        free(frame);
        fprintf(stderr, "Failed creating references for the frame!\n");
        return NULL;
    }
    vm_push_frame(vm, frame);
    return frame;
}

// A wrapper function to push the frame into the VM after its creation.
void vm_push_frame(vm_t *vm, frame_t *frame)
{
    if (vm == NULL || frame == NULL) {
        return;
    }
    push(vm->frames, (void *)frame);
    return;
}

// Free a particular frame.
void free_frame(frame_t *frame)
{
    if (frame == NULL) {
        return;
    }
    // Free frame->references->data.
    // Objects not freed.
    free_stack(frame->references);
    free(frame);
}

// Push an object to the objects stack of the VM.
void vm_track_object(vm_t *vm, object_t *obj)
{
    if (vm == NULL || obj == NULL){
        return;
    }
    push(vm->objects, (void*)obj);
    return;
}

// Push an object into the frame.
// so that we know that an object is referenced inside a frame.
void frame_reference_obj(frame_t *frame, object_t *obj)
{
    if (frame == NULL || obj == NULL) {
        return;
    }
    push(frame->references, (void *)obj);
}

// Iterate over each frame and set is_marked=true for every object that is referenced.
void mark(vm_t *vm)
{
    if (vm == NULL) {
        return;
    }
    // vm->frames <==> Stack of frames.
    // vm->frames->data[i] <==> A frame(stack of references).
    // vm->frames->data[i]->references->data[i] <==> object in the frame.
    for (size_t i = 0; i < vm->frames->top; i++) {
        // NOTE: A frame(a stack element) is pushed as void*
        frame_t *frame = vm->frames->data[i];
        for (size_t j = 0; j < frame->references->top; j++) {
            object_t *obj = frame->references->data[j];
            // Mark the root objects of the frame.
            obj->is_marked = true;
        }
    }
}

// Trace part of the mark phase. Look for objects referenced by the root object
// Root objects -> objects found in the mark phase.

// Collect the root objects so that they can be probed for child objects.
void trace(vm_t *vm)
{
    if (vm == NULL) {
        return;
    }
    stack_t *grey_objects = get_stack(8);
    if (grey_objects == NULL) {
        return;
    }
    for (size_t i = 0; i < vm->objects->top; i++) {
        object_t *obj = vm->objects->data[i];
        // If the object is marked, push it to grey_objects stack.
        // This is done to ensure that we can mark any child objects that this object might have
        // <eg. in arrays>
        if (obj && obj->is_marked) {
            push(grey_objects, obj);  
        }
    }
    /*
      * Avoid: grey_objects can resize; <use pop>
        for (size_t i = 0; i < grey_objects->top; i++) {
            trace_blacken_object(grey_objects, grey_objects->data[i]);
        }
    */
    // Probing the root objects.
    while (grey_objects->top > 0) {
        object_t *obj = pop(grey_objects);
        trace_blacken_object(grey_objects, obj);
    }
    // Free the temporary collection.
    free_stack(grey_objects);
    return;
}

// Trace the root objects for referenced child objects and mark them.
void trace_blacken_object(stack_t *grey_objects, object_t *obj)
{
    if (grey_objects == NULL || obj == NULL) {
        return;
    }
    switch(obj->datatype) {
        case CHARACTER:
        case BOOLEAN:
        case REAL:
        case INTEGER:
        case STRING:
            break;
        case ARRAY:
            // Mark the objects held by the array.
            for (int i = 0; i < obj->value.array.capacity; i++) {
                // Since an array object allows, random index insertions.
                 if (obj->value.array.arr[i] != NULL) {
                    // Mark the objects held by array.
                    trace_mark_object(grey_objects, obj->value.array.arr[i]);
                }
            }
            break;
        default:
            fprintf(stderr, "Invalid Object Read!\n");
    }
    return;
}

// Collect objects which are marked and to check if they can be traversed for sub objects.
// Primary Use: called on grey_objects <collection of root>.
void trace_mark_object(stack_t *grey_objects, object_t *obj)
{
    // If object already marked, do nothing.
    if (grey_objects == NULL || obj == NULL || obj->is_marked) {
        return;
    }
    obj->is_marked = true;
    push(grey_objects, obj);
    return;
}

// Sweep phase of the MnS.
void sweep(vm_t *vm)
{
    if (vm == NULL) {
        return;
    }
    for (size_t i = 0; i < vm->objects->top; i++) {
        // Cast the objects in the vm, and free if not referenced.
        object_t *obj = vm->objects->data[i];
        if (obj->is_marked) {
            // Reset is_marked i.e set them up for the next GC cycle.
            obj->is_marked = false;
        } else {
            free_obj(obj);
            // obj = NULL <not the right thing to do>
            vm->objects->data[i] = NULL;
        }
    }
    // Clean the stack.
    stack_remove_nulls(vm->objects);
    return;
}

// Call the GC.
void run_gc(vm_t *vm)
{
    if (vm == NULL) {
        return;
    }
    mark(vm);
    trace(vm);
    sweep(vm);
    return;
}
