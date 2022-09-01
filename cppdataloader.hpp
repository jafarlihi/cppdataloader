#include <vector>
#include <future>
#include <algorithm>

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

      auto cacheMap = std::vector<std::pair<size_t, size_t>>();
      for (int i = 0; i < ids.size(); i++) {
        int index = std::find(ids.begin(), ids.end(), ids[i]) - ids.begin();
        if (index < i)
          cacheMap.push_back(std::make_pair(i, index));
      }

      for (auto cacheMapping : cacheMap)
        ids.erase(ids.begin() + cacheMapping.first);

      auto results = loader.load(ids);

      for (auto cacheMapping : cacheMap)
        results.insert(results.begin() + cacheMapping.first, results[cacheMapping.second]);

      for (int i = 0; i < results.size(); i++)
        promises[i].second.set_value(results[i]);
    }
  private:
    const BatchLoader<K, V>& loader;
    std::vector<std::pair<K, std::promise<V>>> promises;
};

