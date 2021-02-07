template <typename T> struct Foo {};
struct Bar {};

void Baz() {
  Foo<int> a, b;
  a = b;
  Bar c, d;
  c = d;
}
