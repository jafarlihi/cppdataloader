#include <string>
#include <variant>
#include <vector>
#include <future>
#include <utility>

template<typename K, typename V>
class BatchLoader {
  public:
    virtual std::vector<V> load(std::vector<K> ids) const = 0;
};

template<typename K, typename V>
class DataLoader {
  public:
    DataLoader(const BatchLoader<K, V>& _loader) : loader(_loader) {}

    std::future<V> load(K id) {
      std::promise<V> promise;
      std::future<V> future = promise.get_future();
      promises.push_back(std::make_pair(id, std::move(promise)));
      return future;
    }

    void dispatch() {
      auto ids = std::vector<K>();
      for (auto& pair : promises)
        ids.push_back(pair.first);
      auto results = loader.load(ids);
      for (int i = 0; i < results.size(); i++)
        promises[i].second.set_value(results[i]);
    }
  private:
    const BatchLoader<K, V>& loader;
    std::vector<std::pair<K, std::promise<V>>> promises;
};

class User {
  public:
    User(long _id): id(_id) {}
  private:
    long id;
};

class UserBatchLoader : public BatchLoader<long, User> {
  public:
    std::vector<User> load(std::vector<long> userIds) const override {
      auto result = std::vector<User>();
      // Make a DB call here or something and pass in userIds to SELECT and return
      for (auto userId : userIds)
        result.push_back(User(userId));
      return result;
    }
};

int main(int argc, char *argv[]) {
  UserBatchLoader userBatchLoader = UserBatchLoader();
  DataLoader<long, User> userLoader = DataLoader(userBatchLoader);

  std::future<User> load1 = userLoader.load(1);
  std::future<User> load2 = userLoader.load(2);

  userLoader.dispatch();
}
