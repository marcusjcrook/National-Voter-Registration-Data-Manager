#ifndef HASHER_H
#define HASHER_H

template <typename T>
class Hasher {
public:
    virtual unsigned long hash(const T& item) const = 0; 	// returns a hash code for the item, but may not be within table bounds
};

#endif // !HASHER_H
