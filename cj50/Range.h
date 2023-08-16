#pragma once

/// Represents a range of indices from start (inclusive) to end
/// (exclusive).

/// (Todo: should this be parametrized, for int, uchar etc.?)

typedef struct Range {
    size_t start;
    size_t end;
} Range;

static UNUSED
Range range(size_t start, size_t end) {
    return (Range) {
        .start = start,
        .end = end
    };
}

