const Lexer = @import("lexer.zig").Lexer;

pub const Parser = struct {
    pub fn parse(buffer: []const u8) void {
        var lexer: Lexer = Lexer.init(buffer);
        _ = lexer;
    }
};

pub const Program = struct {};
