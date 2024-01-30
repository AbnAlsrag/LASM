const std = @import("std");
const lvm = @import("lvm");

const STACK_SIZE = 512;

const Lasm = struct {
    lvm: lvm.Machine,

    fn init() Lasm {
        var result: Lasm = Lasm{
            .lvm = lvm.Machine.init(STACK_SIZE),
        };
        return result;
    }

    fn parseLasmFile(self: *Lasm, path: []const u8) void {
        _ = self;
        _ = path;
    }

    fn saveToMelf(self: Lasm, path: []const u8) void {
        self.lvm.saveToFile(path);
    }
};

pub fn main() void {
    const in_path = "test.lasm";
    const out_path = "test.melf";

    var lasm: Lasm = Lasm.init();
    lasm.parseLasmFile(in_path);
    lasm.saveToMelf(out_path);
}
