
// parameters:

//   getc_unlocked(in)
//   POSSIBLY(expr, label) -- use TRY(expr, label) if getc_unlocked returns a Result
//   POSSIBLYDEREF(v) -- dereference what TRY(getc_unlocked(in),..) returned

// context:

//   in: passed to getc_unlocked()

{
    BEGIN_Result(Option(ucodepoint), UnicodeError);
    
    // https://en.wikipedia.org/wiki/Utf-8#Encoding
    u32 codepoint;
    LET_Some_ELSE(_b1, POSSIBLYTRY(getc_unlocked(in), cleanup)) {
        RETURN_Ok(none_ucodepoint(), cleanup);
    }
    char b1 = POSSIBLYDEREF(_b1);
    int numbytes;
    if ((b1 & 128) == 0) {
        // codepoint encoded as a single byte
        numbytes = 1;
        codepoint = b1;
    } else {
        if        ((b1 & 0b11100000) == 0b11000000) {
            numbytes = 2;
            codepoint = b1 & 0b11111;
        } else if ((b1 & 0b11110000) == 0b11100000) {
            numbytes = 3;
            codepoint = b1 & 0b1111;
        } else if ((b1 & 0b11111000) == 0b11110000) {
            numbytes = 4;
            codepoint = b1 & 0b111;
        } else {
            RETURN_Err(DecodingError_InvalidStartByte(),
                       cleanup);
        }
        for (int i = 1; i < numbytes; i++) {
            LET_Some_ELSE(_b, POSSIBLYTRY(getc_unlocked(in), cleanup)) {
                RETURN_Err(DecodingError_PrematureEof(i+1),
                           cleanup);
            }
            char b = POSSIBLYDEREF(_b);
            if ((b & 0b11000000) != 0b10000000) {
                RETURN_Err(DecodingError_InvalidContinuationByte(i+1),
                           cleanup);
            }
            codepoint <<= 6;
            codepoint |= (b & 0b00111111);
        }
    }
    if (codepoint <= 0x10FFFF) {
        AUTO cp = ucodepoint(codepoint);
        int expected_numbytes = utf8_sequence_len_ucodepoint(cp);
        if (expected_numbytes == numbytes) {
            RETURN_Ok(some_ucodepoint(cp),
                      cleanup);
        } else {
            RETURN_Err(DecodingError_OverlongEncoding(codepoint),
                       cleanup);
        }
    } else {
        RETURN_Err(DecodingError_InvalidCodepoint(codepoint),
                   cleanup);
    }
cleanup:
    END_Result();
}
