// parameters: T

// Needed definitions:
//  - slice(T)
//  - Option(ref(T)) (which needs ref(T), obviously)

typedef struct SliceIterator(T) {
    slice(T) slice;
    size_t pos;
} SliceIterator(T);


static UNUSED
SliceIterator(T) XCAT(new_, SliceIterator(T))(slice(T) slice) {
    return (SliceIterator(T)) {
        .slice = slice,
        .pos = 0
    };
}

static UNUSED
void XCAT(drop_, SliceIterator(T))(UNUSED SliceIterator(T) slice) { }


/// Get a reference to the next element in the slice that this
/// iterator is iterating over, or None if the end was reached.

static UNUSED
Option(ref(T)) XCAT(next_, SliceIterator(T))(SliceIterator(T) *self) {
    size_t pos = self->pos;
    if (pos < self->slice.len) {
        const T *item = &self->slice.ptr[pos];
        self->pos = pos + 1;
        return Some(ref(T))(item);
    } else {
        return None(ref(T));
    }
}

