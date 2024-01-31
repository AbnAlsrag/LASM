const std = @import("std");
const lvm = @import("lvm");
const Parser = @import("parser.zig").Parser;
const Program = @import("parser.zig").Program;

const STACK_SIZE = 512;
const LASM_FILE_MAX_SIZE = 5000;

const Lasm = struct {
    lvm: lvm.Machine,

    fn init() Lasm {
        var result: Lasm = Lasm{
            .lvm = lvm.Machine.init(STACK_SIZE),
        };
        return result;
    }

    //TODO: Add better errors
    fn parseLasmFile(self: *Lasm, path: []const u8) void {
        var file = std.fs.cwd().openFile(path, .{}) catch {
            @panic("[ERROR] error while parsing lasm file");
        };
        defer file.close();

        var heap = std.heap.HeapAllocator.init();
        defer heap.deinit();

        const allocator = heap.allocator();

        var buf_reader = std.io.bufferedReader(file.reader());
        var in_stream = buf_reader.reader();

        const buffer: []u8 = in_stream.readAllAlloc(allocator, LASM_FILE_MAX_SIZE) catch {
            @panic("[ERROR] error while parsing lasm file");
        };

        Parser.parse(buffer);
        _ = self;
        allocator.free(buffer);
    }
};

pub fn main() void {
    const in_path = "test.lasm";
    const out_path = "test.melf";
    _ = out_path;

    var lasm: Lasm = Lasm.init();
    lasm.parseLasmFile(in_path);
}
