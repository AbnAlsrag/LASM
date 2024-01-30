const std = @import("std");

pub const Lexer = struct {
    pub const TokenType = enum(u8) {
        name,
        register,
        num,
        comma,
        colon,
        new_line,
        eof,
    };

    pub const Token = struct {
        type: TokenType,
        text: []const u8,

        fn init(typ: TokenType, text: []const u8) Token {
            var result: Token = Token{
                .type = typ,
                .text = text,
            };

            return result;
        }

        pub fn print(self: Token) void {
            std.debug.print("Token[type: {s}, text: {s}]\n", .{ @tagName(self.type), self.text });
        }
    };

    buffer: []const u8,
    current: usize = 0,

    pub fn init(buffer: []const u8) Lexer {
        var result = Lexer{
            .buffer = buffer,
        };

        return result;
    }

    pub fn nexToken(self: *Lexer) Token {
        var result: Token = undefined;

        if (self.current >= self.buffer.len) {
            result = Token.init(.eof, "");
        } else {
            if (isWhiteSpace(self.buffer[self.current])) {
                self.chopWhiteSpace();
            }

            if (self.buffer[self.current] == '#') {
                self.chopComment();
            }

            if (self.buffer[self.current] == '\n') {
                self.current += 1;
                result = Token.init(.new_line, "");
            } else if (isAlpha(self.buffer[self.current])) {
                result = self.parseAlpha();
            } else if (isNum(self.buffer[self.current])) {
                result = self.parseNum();
            } else if (self.buffer[self.current] == ',') {
                self.current += 1;
                result = Token.init(.comma, ",");
            } else if (self.buffer[self.current] == ':') {
                self.current += 1;
                result = Token.init(.colon, ":");
            }
        }

        return result;
    }

    pub fn expectToken(self: *Lexer, token: TokenType) bool {
        const current = self.current;

        const tok = self.nexToken();

        self.current = current;
        if (tok.type == token) {
            return true;
        } else {
            return false;
        }
    }

    fn chopWhiteSpace(self: *Lexer) void {
        while (self.current < self.buffer.len and isWhiteSpace(self.buffer[self.current])) {
            self.current += 1;
        }
    }

    fn chopComment(self: *Lexer) void {
        while (self.current < self.buffer.len and self.buffer[self.current] != '\n') {
            self.current += 1;
        }
    }

    fn parseAlpha(self: *Lexer) Token {
        const begin: usize = self.current;
        while (self.current < self.buffer.len and !isWhiteSpace(self.buffer[self.current]) and isAlpha(self.buffer[self.current])) {
            self.current += 1;
        }

        const end: usize = self.current;

        if (self.buffer[begin..end].len == 2 and self.buffer[begin..end][0] == 'r') {
            return Token.init(.register, self.buffer[begin..end]);
        } else {
            return Token.init(.name, self.buffer[begin..end]);
        }
    }

    fn parseNum(self: *Lexer) Token {
        const begin: usize = self.current;
        while (self.current < self.buffer.len and !isWhiteSpace(self.buffer[self.current]) and isNum(self.buffer[self.current])) {
            self.current += 1;
        }

        const end: usize = self.current;

        return Token.init(.name, self.buffer[begin..end]);
    }

    fn isWhiteSpace(char: u8) bool {
        switch (char) {
            ' ', '\t', '\r', 11, 12 => return true,
            else => return false,
        }
    }

    fn isAlpha(char: u8) bool {
        switch (char) {
            'a'...'z', 'A'...'Z', '_' => return true,
            else => return false,
        }
    }

    fn isNum(char: u8) bool {
        switch (char) {
            '0'...'9' => return true,
            else => return false,
        }
    }
};
