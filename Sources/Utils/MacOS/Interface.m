//
// Interface.m
//

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>

void makeViewMetalCompatible(void* handle);
void* getMetalLayer(void* window);

void makeViewMetalCompatible(void* handle)
{
    NSView* view = (__bridge NSView*)handle;
    assert([view isKindOfClass:[NSView class]]);

    if (![view.layer isKindOfClass:[CAMetalLayer class]])
    {
        [view setLayer:[CAMetalLayer layer]];
        [view setWantsLayer:YES];
    }
}

void* getMetalLayer(void* window)
{
    NSView* view;
    NSWindow* ns_window = (__bridge NSWindow*) window;
    void* viewHandle = (__bridge void*) [ns_window contentView];
    makeViewMetalCompatible(viewHandle);
    view = (__bridge NSView*)viewHandle;
    return (__bridge void*) [view layer];
}
