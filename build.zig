const std = @import("std");

pub fn build(b: *std.build.Builder) void {
    const target = b.standardTargetOptions(.{});
    const mode = b.standardReleaseOptions();

    const exe = b.addExecutable("JackyShooter", "src/main.zig");
    exe.setTarget(target);
    exe.setBuildMode(mode);
    exe.install();

    exe.linkLibC();
    exe.addIncludeDir("/usr/local/include");
    exe.addIncludeDir("/usr/include");
    exe.addIncludeDir("lib");

    exe.addPackagePath("zgl", "lib/zgl/zgl.zig");
    exe.addPackagePath("mach-glfw", "lib/mach-glfw/src/main.zig");
    exe.addPackagePath("zlm", "lib/zlm/zlm.zig");

    exe.linkSystemLibrary("glfw");
    exe.linkSystemLibrary("epoxy");

    exe.addCSourceFile("lib/stb_image.c", &.{});

    const run_cmd = exe.run();
    run_cmd.step.dependOn(b.getInstallStep());
    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);

    const exe_tests = b.addTest("src/main.zig");
    exe_tests.setTarget(target);
    exe_tests.setBuildMode(mode);

    const test_step = b.step("test", "Run unit tests");
    test_step.dependOn(&exe_tests.step);
}
