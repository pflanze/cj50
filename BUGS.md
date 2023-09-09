# Known bugs

## Documentation

* The documentation is meant to show the struct definitions, but
  currently doesn't if the struct type is parameterized.

* Generics with nested application of `_Generic` (e.g. in
  `new_from.h`) are not currently properly handled. Also, fake entries
  are not filtered out currently.

## Library design

* `utf8char` should get lower importance that it appears to get;
  `ucodepoint` should be preferred. Just document properly?
  
* Todo: `uchar` (not implemented yet except for the constructor macro
  that actually returns a `ucodepoint`) will be holding
  character-encoding codepoints only, as in Rust. Conversion from
  ucodepoint. Once done, `uchar` should be the normal unicode
  character for users, and `ucodepoint` should not normally be used
  any more.

## Random items

* Random number generation design should not be counter performance;
  re-initializing as per the `__APPLE__` code is probably not good.

