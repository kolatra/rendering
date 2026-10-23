#import <Cocoa/Cocoa.h>

#include <engine/platform/WindowNS.hpp>
#include <engine/core/Config.hpp>

@interface AppDelegate : NSObject<NSApplicationDelegate> {
    NSWindow *window;
}

- (void) makeWindow;
@end

@implementation AppDelegate

- (void) applicationDidFinishLaunching: (NSNotification *) notif {
    [self makeWindow];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
    return YES;
}

- (void) makeWindow {
    Luna::Configuration *config = new Luna::Configuration();
    Luna::MacOSWindow *w = new Luna::MacOSWindow(config);
    window = (__bridge NSWindow *) w->GetNativeHandle();
}
@end

int main(int argc, const char *argv[]) {
    @autoreleasepool {
        NSApplication *app = [NSApplication sharedApplication];
        [app setActivationPolicy:NSApplicationActivationPolicyRegular];
        
        AppDelegate *delegate = [[AppDelegate alloc] init];
        [app setDelegate:delegate];
        
        [app run];
    }
    return 0;
}
