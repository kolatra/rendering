#import <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject<NSApplicationDelegate> {
    NSWindow *window;
    NSTextView *textView;
    NSTextField *textField;
    NSButton *button;
}

- (void) makeWindow;
@end

@implementation AppDelegate

- (void) applicationDidFinishLaunching: (NSNotification *) not {
    [self makeWindow];
}

- (void) makeWindow {
    window = [[NSWindow alloc] initWithContentRect: NSMakeRect(400,400,400,200)
                                         styleMask: NSWindowStyleMaskTitled |
                                                    NSResizableWindowMask |
                                                    NSClosableWindowMask |
                                                    NSMiniaturizableWindowMask
                                           backing: NSBackingStoreBuffered
                                             defer: NO];

    [window setTitle:@"NS Application"];
    [window makeKeyAndOrderFront:NSApp];
}
@end

int main(int argc, const char *argv[]) {
    NSLog(@"Welcome to objective-c!");
    [NSApplication sharedApplication];
    AppDelegate *appDelegate = [AppDelegate new];
    [NSApp setDelegate:appDelegate];
    [NSApp run];
    return 0;
}
