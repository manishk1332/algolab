#include <iostream>
using namespace std;

// Insertion Sort
void insertionSort(int arr[], int n) {
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// Bubble Sort
void bubbleSort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                // Swap arr[j] and arr[j+1]
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

// Selection Sort
void selectionSort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int minIndex = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j] < arr[minIndex])
                minIndex = j;
        }
        // Swap arr[i] and arr[minIndex]
        int temp = arr[i];
        arr[i] = arr[minIndex];
        arr[minIndex] = temp;
    }
}

// Merge function for Merge Sort
void merge(int arr[], int left, int mid, int right) {
    int n1 = mid - left + 1;  // Size of left subarray
    int n2 = right - mid;     // Size of right subarray

    // Dynamically allocate temporary arrays
    int* leftArr = new int[n1];
    int* rightArr = new int[n2];

    // Copy data into temporary arrays
    for (int i = 0; i < n1; i++) {
        leftArr[i] = arr[left + i];
    }
    for (int j = 0; j < n2; j++) {
        rightArr[j] = arr[mid + 1 + j];
    }

    // Merge the temporary arrays back into arr[left...right]
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (leftArr[i] <= rightArr[j])
            arr[k++] = leftArr[i++];
        else
            arr[k++] = rightArr[j++];
    }
    // Copy any remaining elements of leftArr
    while (i < n1)
        arr[k++] = leftArr[i++];
    // Copy any remaining elements of rightArr
    while (j < n2)
        arr[k++] = rightArr[j++];

    // Free the dynamically allocated memory
    delete[] leftArr;
    delete[] rightArr;
}

// Merge Sort
void mergeSort(int arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

// Partition function for Quick Sort
int partition(int arr[], int left, int right) {
    int pivot = arr[right];  // Choose the last element as pivot
    int i = left - 1;
    for (int j = left; j < right; j++) {
        if (arr[j] < pivot) {
            i++;
            // Swap arr[i] and arr[j]
            int temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    // Swap arr[i+1] and arr[right] (pivot)
    int temp = arr[i + 1];
    arr[i + 1] = arr[right];
    arr[right] = temp;
    return i + 1;
}

// Quick Sort
void quickSort(int arr[], int left, int right) {
    if (left < right) {
        int pivotIndex = partition(arr, left, right);
        quickSort(arr, left, pivotIndex - 1);
        quickSort(arr, pivotIndex + 1, right);
    }
}

int main() {
    int n;
    cout << "Enter the number of elements: ";
    cin >> n;

    // Dynamically allocate the main array and read input values
    int* arr = new int[n];
    cout << "Enter " << n << " elements:" << endl;
    for (int i = 0; i < n; i++) {
        cin >> arr[i];
    }

    // Create separate copies for each sorting algorithm
    int* arrInsertion = new int[n];
    int* arrBubble = new int[n];
    int* arrSelection = new int[n];
    int* arrMerge = new int[n];
    int* arrQuick = new int[n];

    for (int i = 0; i < n; i++) {
        arrInsertion[i] = arr[i];
        arrBubble[i] = arr[i];
        arrSelection[i] = arr[i];
        arrMerge[i] = arr[i];
        arrQuick[i] = arr[i];
    }

    // Sort using different algorithms
    insertionSort(arrInsertion, n);
    bubbleSort(arrBubble, n);
    selectionSort(arrSelection, n);
    mergeSort(arrMerge, 0, n - 1);
    quickSort(arrQuick, 0, n - 1);

    // Display the sorted arrays
    cout << "Sorted array using Insertion Sort: ";
    for (int i = 0; i < n; i++) {
        cout << arrInsertion[i] << " ";
    }
    cout << endl;

    cout << "Sorted array using Bubble Sort: ";
    for (int i = 0; i < n; i++) {
        cout << arrBubble[i] << " ";
    }
    cout << endl;

    cout << "Sorted array using Selection Sort: ";
    for (int i = 0; i < n; i++) {
        cout << arrSelection[i] << " ";
    }
    cout << endl;

    cout << "Sorted array using Merge Sort: ";
    for (int i = 0; i < n; i++) {
        cout << arrMerge[i] << " ";
    }
    cout << endl;

    cout << "Sorted array using Quick Sort: ";
    for (int i = 0; i < n; i++) {
        cout << arrQuick[i] << " ";
    }
    cout << endl;

    // Free all dynamically allocated memory
    delete[] arr;
    delete[] arrInsertion;
    delete[] arrBubble;
    delete[] arrSelection;
    delete[] arrMerge;
    delete[] arrQuick;

    return 0;
}