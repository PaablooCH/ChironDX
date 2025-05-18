#pragma once
#include <unordered_map>
#include <optional>

namespace Chiron
{
    template <typename Key, typename Value>
    class BidirectionalMap
    {
    public:
        inline void Insert(const Key& key, const Value& value);

        inline void UpdateByKey(const Key& key, const Value& value);
        inline void UpdateByValue(const Value& value, const Key& key);

        inline void EraseByKey(const Key& key);
        inline void EraseByValue(const Value& value);

        inline std::optional<Value> GetValue(const Key& key) const;
        inline std::optional<Key> GetKey(const Value& value) const;

        inline void Clear();

        inline bool ContainsKey(const Key& key) const;
        inline bool ContainsValue(const Value& value) const;

    private:
        std::unordered_map<Key, Value> _keyToValue;
        std::unordered_map<Value, Key> _valueToKey;
    };

    template<typename Key, typename Value>
    inline void BidirectionalMap<Key, Value>::Insert(const Key& key, const Value& value)
    {
        _keyToValue[key] = value;
        _valueToKey[value] = key;
    }

    template<typename Key, typename Value>
    inline void BidirectionalMap<Key, Value>::UpdateByKey(const Key& key, const Value& value)
    {
        auto it = _keyToValue.find(key);
        if (it != _keyToValue.end())
        {
            _valueToKey.erase(it->second);
            Insert(key, value);
        }
    }

    template<typename Key, typename Value>
    inline void BidirectionalMap<Key, Value>::UpdateByValue(const Value& value, const Key& key)
    {
        auto it = _valueToKey.find(value);
        if (it != _valueToKey.end())
        {
            _keyToValue.erase(it->second);
            Insert(key, value);
        }
    }

    template<typename Key, typename Value>
    inline void BidirectionalMap<Key, Value>::EraseByKey(const Key& key)
    {
        auto it = _keyToValue.find(key);
        if (it != _keyToValue.end())
        {
            _valueToKey.erase(it->second);
            _keyToValue.erase(it);
        }
    }

    template<typename Key, typename Value>
    inline void BidirectionalMap<Key, Value>::EraseByValue(const Value& value)
    {
        auto it = _valueToKey.find(value);
        if (it != _valueToKey.end())
        {
            _keyToValue.erase(it->second);
            _valueToKey.erase(it);
        }
    }

    template<typename Key, typename Value>
    inline std::optional<Value> BidirectionalMap<Key, Value>::GetValue(const Key& key) const
    {
        auto it = _keyToValue.find(key);
        if (it != _keyToValue.end())
        {
            return it->second;
        }
        return std::nullopt;
    }

    template<typename Key, typename Value>
    inline std::optional<Key> BidirectionalMap<Key, Value>::GetKey(const Value& value) const
    {
        auto it = _valueToKey.find(value);
        if (it != _valueToKey.end())
        {
            return it->second;
        }
        return std::nullopt;
    }

    template<typename Key, typename Value>
    inline void BidirectionalMap<Key, Value>::Clear()
    {
        _keyToValue.clear();
        _valueToKey.clear();
    }

    template<typename Key, typename Value>
    inline bool BidirectionalMap<Key, Value>::ContainsKey(const Key& key) const
    {
        return _keyToValue.find(key) != _keyToValue.end();
    }

    template<typename Key, typename Value>
    inline bool BidirectionalMap<Key, Value>::ContainsValue(const Value& value) const
    {
        return _valueToKey.find(value) != _valueToKey.end();
    }
}
