#include <engine/platform/WindowNS.hpp>
#include <engine/core/Config.hpp>
#import <Cocoa/Cocoa.h>

static bool willClose = false;

@interface WindowDelegate : NSObject<NSWindowDelegate>
@end

@implementation WindowDelegate
- (void) windowWillClose:(NSNotification *)notification {
    printf("macOS window closing.\n");
    willClose = true;
}
@end

namespace Luna {
MacOSWindow::MacOSWindow(Configuration *config) {
    @autoreleasepool {
        NSRect frame = NSMakeRect(0, 0, config->width, config->height);
        
        NSWindowStyleMask styleMask = NSWindowStyleMaskTitled |
                                    NSWindowStyleMaskClosable |
                                    NSWindowStyleMaskResizable |
                                    NSWindowStyleMaskMiniaturizable;
        
        NSWindow *window = [[NSWindow alloc] initWithContentRect: frame
                                                       styleMask: styleMask
                                                         backing: NSBackingStoreBuffered
                                                           defer: NO];
        
        [window setTitle:[NSString stringWithUTF8String:config->name]];
        [window center];
        
        WindowDelegate *delegate = [[WindowDelegate alloc] init];
        [window setDelegate:delegate];
        
        nsWindow = (__bridge_retained void*)window;
        
        [window makeKeyAndOrderFront:nil];
    }
}

void MacOSWindow::PollEvents() {}

void MacOSWindow::SwapBuffers() {}

double MacOSWindow::GetTime() {}

void *MacOSWindow::GetNativeHandle() {
    return nsWindow;
}

bool MacOSWindow::ShouldClose() {
    return willClose;
}

void MacOSWindow::Show() {
    
}

}
