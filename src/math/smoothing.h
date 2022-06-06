#pragma once

/**
 * @brief Input smoother for analog inputs
 *
 * This code is based on the public domain code found here:
 * https://docs.arduino.cc/built-in-examples/analog/Smoothing
 *
 */

#define SMOOTHING_BUCKET_SIZE 22


namespace creatures
{

    template <typename T>
    class InputSmoother
    {
    public:
        InputSmoother<T>();

        void addValue(T value);
        T getCurrentValue();

    private:
        int bucketSize;
        T total;
        int currentIndex;
        T storageArray[SMOOTHING_BUCKET_SIZE];
    };


    template <typename T>
    InputSmoother<T>::InputSmoother()
    {
        this->total = 0;
        this->currentIndex = 0;

        // Make sure the bucket is zero to start
        for(int i = 0; i < SMOOTHING_BUCKET_SIZE; i++)
        {
            storageArray[i] = 0;
        }

        l.verbose("created a new smoother with a bucket size of %d", SMOOTHING_BUCKET_SIZE);
    }

    template <typename T>
    void InputSmoother<T>::addValue(T value)
    {
        // subtract the last reading:
        total = total - storageArray[currentIndex];

        // Same this value
        storageArray[currentIndex] = value;

        // add the reading to the total:
        total = total + storageArray[currentIndex];

        // advance to the next position in the array:
        currentIndex++;

        // if we're at the end of the array...
        if (currentIndex >= SMOOTHING_BUCKET_SIZE)
        {
            // ...wrap around to the beginning:
            currentIndex = 0;
        }

        l.verbose("added a value, %d, currentIndex: %d", value, currentIndex);
    }

    template <typename T>
    T InputSmoother<T>::getCurrentValue()
    {
        return total / SMOOTHING_BUCKET_SIZE;
    }

}