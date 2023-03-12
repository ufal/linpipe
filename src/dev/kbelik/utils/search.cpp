#include "common.h"
#include "algorithm"

using namespace linpipe;

int binary(int arr[], int array_size, int val) {
  int arr_access, l, r, m;
  arr_access = 0;
  l = 0;
  r = array_size - 1;
  while (l <= r) {
    m = (l + r) / 2;
    arr_access++;
    int mv = arr[m];
    if (mv < val) 
      l = m + 1;
    else if (mv > val)
      r = m - 1;
    else
      break;
  }
  return arr_access;
}

int expo(int arr[], int array_size, int val, int jump_size) {
  int arr_access, r;
  arr_access = 0;
  r = 1;
  while (r <= array_size) {
    int mv = arr[r];
    arr_access++;
    if (mv < val)
      r *= jump_size;
    else if (mv > val) {
      int l = r / jump_size;
      arr_access += binary(arr + l, r - l, val);
      return arr_access;
    }
    else 
      return arr_access;
  }
  return arr_access;
}

int expo2(int arr[], int array_size, int val) { 
  return expo(arr, array_size, val, 2);
}

int expo3(int arr[], int array_size, int val) { 
  return expo(arr, array_size, val, 3);
}

int expo4(int arr[], int array_size, int val) { 
  return expo(arr, array_size, val, 4);
}

int expo8(int arr[], int array_size, int val) { 
  return expo(arr, array_size, val, 8);
}

int expo64(int arr[], int array_size, int val) { 
  return expo(arr, array_size, val, 64);
}

int interpolate_val(int l, int r, int lv, int rv, int val) {
  int wanted = val - lv;
  int pos = wanted * ((r - l) / (double)(rv - lv));
  return pos + l;
}

int interpolated(int arr[], int array_size, int val) {
  int arr_access, l, r, lv, rv;
  arr_access = 0;
  l = 0;
  r = array_size - 1;
  while (l <= r) {
    // Sets boundary values lv and rv.
    // Terminates if val is found.
    for (auto p : {pair<int*, int>(&lv, l), pair<int*, int>(&rv, r)}) {
      *p.first = arr[p.second];
      arr_access++;
      if (*p.first == val)
        return arr_access;
    }
    int m = interpolate_val(l, r, lv, rv, val);
    //cout <<"lv rv val " << lv << ' ' << rv << ' ' << val << endl;
    if (lv > val || rv < val)
      return arr_access;
    int mv = arr[m];
    arr_access++;
    //cout << lv << ' '<<rv  << ' '<< m << ' ' << l << ' '<< r<< ' ' << val<< ' ' << arr[m] << endl;
    if (mv < val)
      l = m + 1;
    else if (mv > val)
      r = m - 1;
    else
      return arr_access;
  }
  return arr_access;
}
