# cppdataloader

**Warning**
cppdataloader is still alpha and stability is not guaranteed.

cppdataloader is a batching and caching library for C++17.

cppdataloader is a generic utility to be used as part of your application's data fetching layer to provide a simplified and consistent API over various remote data sources such as databases or web services via batching and caching.

## Features
- [x] Batching
- [x] Caching
- [x] Direct dispatch
- [ ] Timeout dispatch
- [ ] Empty values

## Quickstart

cppq is a header-only library with no dependencies.

Just include the header: `#include "cppdataloader.h"`.

## Example

```c++
#include "cppdataloader.hpp"

#include <iostream>

class User {
  public:
    User(long _id): id(_id) {}
    long id;
};

class UserBatchLoader : public BatchLoader<long, User> {
  public:
    std::vector<User> load(std::vector<long> userIds) const override {
      auto result = std::vector<User>();
      // Make a DB call here or something and pass in userIds to SELECT and return
      for (auto userId : userIds)
        result.push_back(User(userId));
      // Results should be in the same order as passed-in IDs
      return result;
    }
};

int main(int argc, char *argv[]) {
  UserBatchLoader userBatchLoader = UserBatchLoader();
  DataLoader<long, User> userLoader = DataLoader(userBatchLoader);

  std::future<User> load1 = userLoader.load(1);
  std::future<User> load2 = userLoader.load(2);
  // Since this fetches the same ID of `1`, it will return the cached value
  std::future<User> load3 = userLoader.load(1);

  // Both calls to `load` will be batched and executed with this call
  userLoader.dispatch();

  std::cout << load1.get().id << " " << load2.get().id << " " << load3.get().id << std::endl;
}
```

## License

cppdataloader is MIT-licensed.
