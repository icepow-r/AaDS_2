#include <iostream>

class TimsortArray
{
   private:
    int* data;
    int capacity;
    int size;

    void resize(int newCapacity)
    {
        int* newData = new int[newCapacity];
        for (int i = 0; i < size; i++)
        {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
        capacity = newCapacity;
    }

   public:
    TimsortArray() : data(nullptr), capacity(0), size(0) {}

    TimsortArray(int cap) : capacity(cap), size(0) { data = new int[capacity]; }

    ~TimsortArray() { delete[] data; }

    void push(const int& value)
    {
        if (size >= capacity)
        {
            resize(capacity == 0 ? 1 : capacity * 2);
        }
        data[size++] = value;
    }

    int& operator[](int index) { return data[index]; }

    const int& operator[](int index) const { return data[index]; }

    int getSize() const { return size; }

    void setSize(int newSize) { size = newSize; }

    int* getData() { return data; }
};

struct Run
{
    int start;
    int length;
};

class RunStack
{
   private:
    Run* data;
    int capacity;
    int size;

    void resize(int newCapacity)
    {
        Run* newData = new Run[newCapacity];
        for (int i = 0; i < size; i++)
        {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
        capacity = newCapacity;
    }

   public:
    RunStack() : data(nullptr), capacity(0), size(0) {}

    RunStack(int cap) : capacity(cap), size(0) { data = new Run[capacity]; }

    ~RunStack() { delete[] data; }

    void push(const Run& value)
    {
        if (size >= capacity)
        {
            resize(capacity == 0 ? 8 : capacity * 2);
        }
        data[size++] = value;
    }

    Run pop() { return data[--size]; }

    Run& top() { return data[size - 1]; }

    Run& operator[](int index) { return data[index]; }

    int getSize() const { return size; }

    bool empty() const { return size == 0; }

    void setSize(int newSize) { size = newSize; }
};

int computeMinrun(int n)
{
    int r = 0;
    while (n >= 64)
    {
        r |= n & 1;
        n >>= 1;
    }
    return n + r;
}

void insertionSort(int arr[], int left, int right)
{
    for (int i = left + 1; i <= right; i++)
    {
        int key = arr[i];
        int j = i - 1;
        while (j >= left && arr[j] > key)
        {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

int gallopLeft(int arr[], int key, int base, int len, int hint)
{
    int lastOfs = 0;
    int ofs = 1;

    if (arr[base + hint] < key)
    {
        int maxOfs = len - hint;
        while (ofs < maxOfs && arr[base + hint + ofs] < key)
        {
            lastOfs = ofs;
            ofs = (ofs << 1) + 1;
            if (ofs <= 0)
            {
                ofs = maxOfs;
            }
        }
        if (ofs > maxOfs)
        {
            ofs = maxOfs;
        }
        lastOfs += hint;
        ofs += hint;
    }
    else
    {
        int maxOfs = hint + 1;
        while (ofs < maxOfs && arr[base + hint - ofs] >= key)
        {
            lastOfs = ofs;
            ofs = (ofs << 1) + 1;
            if (ofs <= 0)
            {
                ofs = maxOfs;
            }
        }
        if (ofs > maxOfs)
        {
            ofs = maxOfs;
        }
        int tmp = lastOfs;
        lastOfs = hint - ofs;
        ofs = hint - tmp;
    }

    lastOfs++;
    while (lastOfs < ofs)
    {
        int m = lastOfs + ((ofs - lastOfs) >> 1);
        if (arr[base + m] < key)
        {
            lastOfs = m + 1;
        }
        else
        {
            ofs = m;
        }
    }
    return ofs;
}

int gallopRight(int arr[], int key, int base, int len, int hint)
{
    int ofs = 1;
    int lastOfs = 0;

    if (key < arr[base + hint])
    {
        int maxOfs = hint + 1;
        while (ofs < maxOfs && key < arr[base + hint - ofs])
        {
            lastOfs = ofs;
            ofs = (ofs << 1) + 1;
            if (ofs <= 0)
            {
                ofs = maxOfs;
            }
        }
        if (ofs > maxOfs)
        {
            ofs = maxOfs;
        }
        int tmp = lastOfs;
        lastOfs = hint - ofs;
        ofs = hint - tmp;
    }
    else
    {
        int maxOfs = len - hint;
        while (ofs < maxOfs && key >= arr[base + hint + ofs])
        {
            lastOfs = ofs;
            ofs = (ofs << 1) + 1;
            if (ofs <= 0)
            {
                ofs = maxOfs;
            }
        }
        if (ofs > maxOfs)
        {
            ofs = maxOfs;
        }
        lastOfs += hint;
        ofs += hint;
    }

    lastOfs++;
    while (lastOfs < ofs)
    {
        int m = lastOfs + ((ofs - lastOfs) >> 1);
        if (key < arr[base + m])
        {
            ofs = m;
        }
        else
        {
            lastOfs = m + 1;
        }
    }
    return ofs;
}

void mergeLo(int arr[], int base1, int len1, int base2, int len2)
{
    TimsortArray tmp(len1);
    for (int i = 0; i < len1; i++)
    {
        tmp.push(arr[base1 + i]);
    }

    int cursor1 = 0;
    int cursor2 = base2;
    int dest = base1;

    arr[dest++] = arr[cursor2++];
    if (--len2 == 0)
    {
        for (int i = 0; i < len1; i++)
        {
            arr[dest + i] = tmp[cursor1 + i];
        }
        return;
    }
    if (len1 == 1)
    {
        for (int i = 0; i < len2; i++)
        {
            arr[dest + i] = arr[cursor2 + i];
        }
        arr[dest + len2] = tmp[cursor1];
        return;
    }

    int minGallop = 7;
    bool done = false;

    while (!done)
    {
        int count1 = 0;
        int count2 = 0;

        do
        {
            if (arr[cursor2] < tmp[cursor1])
            {
                arr[dest++] = arr[cursor2++];
                count2++;
                count1 = 0;
                if (--len2 == 0)
                {
                    done = true;
                    break;
                }
            }
            else
            {
                arr[dest++] = tmp[cursor1++];
                count1++;
                count2 = 0;
                if (--len1 == 1)
                {
                    done = true;
                    break;
                }
            }
        } while ((count1 | count2) < minGallop && !done);

        if (done) break;

        do
        {
            count1 = gallopRight(tmp.getData(), arr[cursor2], cursor1, len1, 0);
            if (count1 != 0)
            {
                for (int i = 0; i < count1; i++)
                {
                    arr[dest++] = tmp[cursor1++];
                }
                len1 -= count1;
                if (len1 <= 1)
                {
                    done = true;
                    break;
                }
            }
            arr[dest++] = arr[cursor2++];
            if (--len2 == 0)
            {
                done = true;
                break;
            }

            count2 = gallopLeft(arr, tmp[cursor1], cursor2, len2, 0);
            if (count2 != 0)
            {
                for (int i = 0; i < count2; i++)
                {
                    arr[dest++] = arr[cursor2++];
                }
                len2 -= count2;
                if (len2 == 0)
                {
                    done = true;
                    break;
                }
            }
            arr[dest++] = tmp[cursor1++];
            if (--len1 == 1)
            {
                done = true;
                break;
            }
            minGallop--;
        } while (count1 >= 7 || count2 >= 7 && !done);

        if (!done)
        {
            if (minGallop < 0)
            {
                minGallop = 0;
            }
            minGallop += 2;
        }
    }

    if (len1 == 1)
    {
        for (int i = 0; i < len2; i++)
        {
            arr[dest + i] = arr[cursor2 + i];
        }
        arr[dest + len2] = tmp[cursor1];
    }
    else if (len1 == 0)
    {
        throw std::runtime_error("Comparison error");
    }
    else
    {
        for (int i = 0; i < len1; i++)
        {
            arr[dest + i] = tmp[cursor1 + i];
        }
    }
}

void mergeHi(int arr[], int base1, int len1, int base2, int len2)
{
    TimsortArray tmp(len2);
    for (int i = 0; i < len2; i++)
    {
        tmp.push(arr[base2 + i]);
    }

    int cursor1 = base1 + len1 - 1;
    int cursor2 = len2 - 1;
    int dest = base2 + len2 - 1;

    arr[dest--] = arr[cursor1--];
    if (--len1 == 0)
    {
        for (int i = 0; i < len2; i++)
        {
            arr[dest - len2 + 1 + i] = tmp[i];
        }
        return;
    }
    if (len2 == 1)
    {
        dest -= len1;
        cursor1 -= len1;
        for (int i = len1 - 1; i >= 0; i--)
        {
            arr[dest + 1 + i] = arr[cursor1 + 1 + i];
        }
        arr[dest] = tmp[cursor2];
        return;
    }

    int minGallop = 7;
    bool done = false;

    while (!done)
    {
        int count1 = 0;
        int count2 = 0;

        do
        {
            if (tmp[cursor2] < arr[cursor1])
            {
                arr[dest--] = arr[cursor1--];
                count1++;
                count2 = 0;
                if (--len1 == 0)
                {
                    done = true;
                    break;
                }
            }
            else
            {
                arr[dest--] = tmp[cursor2--];
                count2++;
                count1 = 0;
                if (--len2 == 1)
                {
                    done = true;
                    break;
                }
            }
        } while ((count1 | count2) < minGallop && !done);

        if (done) break;

        do
        {
            count1 = len1 - gallopRight(arr, tmp[cursor2], base1, len1, len1 - 1);
            if (count1 != 0)
            {
                dest -= count1;
                cursor1 -= count1;
                len1 -= count1;
                for (int i = count1 - 1; i >= 0; i--)
                {
                    arr[dest + 1 + i] = arr[cursor1 + 1 + i];
                }
                if (len1 == 0)
                {
                    done = true;
                    break;
                }
            }
            arr[dest--] = tmp[cursor2--];
            if (--len2 == 1)
            {
                done = true;
                break;
            }

            count2 = len2 - gallopLeft(tmp.getData(), arr[cursor1], 0, len2, len2 - 1);
            if (count2 != 0)
            {
                dest -= count2;
                cursor2 -= count2;
                len2 -= count2;
                for (int i = 0; i < count2; i++)
                {
                    arr[dest + 1 + i] = tmp[cursor2 + 1 + i];
                }
                if (len2 <= 1)
                {
                    done = true;
                    break;
                }
            }
            arr[dest--] = arr[cursor1--];
            if (--len1 == 0)
            {
                done = true;
                break;
            }
            minGallop--;
        } while (count1 >= 7 || count2 >= 7 && !done);

        if (!done)
        {
            if (minGallop < 0)
            {
                minGallop = 0;
            }
            minGallop += 2;
        }
    }

    if (len2 == 1)
    {
        dest -= len1;
        cursor1 -= len1;
        for (int i = len1 - 1; i >= 0; i--)
        {
            arr[dest + 1 + i] = arr[cursor1 + 1 + i];
        }
        arr[dest] = tmp[cursor2];
    }
    else if (len2 == 0)
    {
        throw std::runtime_error("Comparison error");
    }
    else
    {
        for (int i = 0; i < len2; i++)
        {
            arr[dest - len2 + 1 + i] = tmp[i];
        }
    }
}

void mergeAt(int arr[], RunStack& stack, int i)
{
    Run run1 = stack[i];
    Run run2 = stack[i + 1];

    stack[i].length = run1.length + run2.length;
    if (i == stack.getSize() - 3)
    {
        stack[i + 1] = stack[i + 2];
    }
    stack.setSize(stack.getSize() - 1);

    int k = gallopRight(arr, arr[run2.start], run1.start, run1.length, 0);
    run1.start += k;
    run1.length -= k;
    if (run1.length == 0)
    {
        return;
    }

    run2.length = gallopLeft(arr, arr[run1.start + run1.length - 1], run2.start, run2.length,
                             run2.length - 1);
    if (run2.length == 0)
    {
        return;
    }

    if (run1.length <= run2.length)
    {
        mergeLo(arr, run1.start, run1.length, run2.start, run2.length);
    }
    else
    {
        mergeHi(arr, run1.start, run1.length, run2.start, run2.length);
    }
}

void mergeCollapse(int arr[], RunStack& stack)
{
    while (stack.getSize() > 1)
    {
        int n = stack.getSize() - 2;
        if (n > 0 && stack[n - 1].length <= stack[n].length + stack[n + 1].length)
        {
            if (stack[n - 1].length < stack[n + 1].length)
            {
                n--;
            }
            mergeAt(arr, stack, n);
        }
        else if (stack[n].length <= stack[n + 1].length)
        {
            mergeAt(arr, stack, n);
        }
        else
        {
            break;
        }
    }
}

void mergeForceCollapse(int arr[], RunStack& stack)
{
    while (stack.getSize() > 1)
    {
        int n = stack.getSize() - 2;
        if (n > 0 && stack[n - 1].length < stack[n + 1].length)
        {
            n--;
        }
        mergeAt(arr, stack, n);
    }
}

int countRunAndMakeAscending(int arr[], int lo, int hi)
{
    int runHi = lo + 1;
    if (runHi == hi)
    {
        return 1;
    }

    if (arr[runHi++] < arr[lo])
    {
        while (runHi < hi && arr[runHi] < arr[runHi - 1])
        {
            runHi++;
        }
        int left = lo;
        int right = runHi - 1;
        while (left < right)
        {
            int tmp = arr[left];
            arr[left] = arr[right];
            arr[right] = tmp;
            left++;
            right--;
        }
    }
    else
    {
        while (runHi < hi && arr[runHi] >= arr[runHi - 1])
        {
            runHi++;
        }
    }

    return runHi - lo;
}

void timsort(int arr[], int n)
{
    if (n < 2)
    {
        return;
    }

    int minRun = computeMinrun(n);
    RunStack stack;

    int lo = 0;
    int hi = n;
    int nRemaining = hi;

    do
    {
        int runLen = countRunAndMakeAscending(arr, lo, hi);

        if (runLen < minRun)
        {
            int force = nRemaining <= minRun ? nRemaining : minRun;
            insertionSort(arr, lo, lo + force - 1);
            runLen = force;
        }

        Run run;
        run.start = lo;
        run.length = runLen;
        stack.push(run);
        mergeCollapse(arr, stack);

        lo += runLen;
        nRemaining -= runLen;
    } while (nRemaining != 0);

    mergeForceCollapse(arr, stack);
}

int main()
{
    std::cout << "Enter the size of the array: ";
    int n;
    if (!(std::cin >> n) || n < 0)
    {
        return 0;
    }
    std::cout << "Enter the elements of the array: ";
    int* arr = new int[n];
    for (int i = 0; i < n; i++)
    {
        if (!(std::cin >> arr[i]))
        {
            arr[i] = 0;
        }
    }
    timsort(arr, n);
    std::cout << "Sorted array: ";
    for (int i = 0; i < n; i++)
    {
        if (i) std::cout << ' ';
        std::cout << arr[i];
    }
    std::cout << std::endl;
    delete[] arr;
    return 0;
}
