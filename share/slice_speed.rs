fn main() {
    let mut v = Vec::new();

    for i in 0..10000000 {
        v.push(i);
    }

    dbg!(v.len());
}
