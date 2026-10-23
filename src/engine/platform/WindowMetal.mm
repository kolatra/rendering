#include <engine/core/Application.hpp>
#include <engine/platform/WindowNS.hpp>
#include <engine/core/Config.hpp>
#import <Cocoa/Cocoa.h>

static bool willClose = false;
static CFGregorianDate* timer;

@interface WindowDelegate : NSObject<NSWindowDelegate>
@end

@implementation WindowDelegate
- (void) windowWillClose:(NSNotification *)notification {
    printf("macOS window closing.\n");
    willClose = true;
}
@end

namespace Luna {
MacOSWindow::MacOSWindow(Configuration *config, Application *app) {
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

        timer = new CFGregorianDate();
    }
}

MacOSWindow::~MacOSWindow() {

}

void MacOSWindow::HandleInput(EngineCamera *camera, float delta) {

}

void *MacOSWindow::CreateSurface(void *apiContext) {
    return nullptr;
}

void MacOSWindow::PollEvents() {}

void MacOSWindow::SwapBuffers() {}

double MacOSWindow::GetTime() {
    return timer->second;
}

void *MacOSWindow::GetNativeHandle() {
    return nsWindow;
}

bool MacOSWindow::ShouldClose() {
    return willClose;
}

void MacOSWindow::Show() {
    [(NSWindow*)CFBridgingRelease(nsWindow) makeKeyAndOrderFront:nil];
}

}
