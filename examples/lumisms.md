> Inspired by Uiua

1. **Reverse each row of an array.**

```python
array = [[1, 2, 3], [4, 5, 6]]
reversed_array = [row[::-1] for row in array]
print(reversed_array)
```

2. **Calculate the sum of an array.**

```python
array = [1, 2, 3, 4, 5]
array_sum = sum(array)
print(array_sum)
```

3. **Determine the product of an array.**

```python
array = [1, 2, 3, 4, 5]
array_product = 1
for num in array:
    array_product *= num
print(array_product)
```

4. **Find the maximum value in an array.**

```python
array = [1, 4, 5, 3, 2]
max_value = max(array)
print(max_value)
```

5. **Find the minimum value in an array.**

```python
array = [1, 4, 5, 3, 2]
min_value = min(array)
print(min_value)
```

6. **Sort an array.**

```python
my_list = [1, 4, 5, 3, 2]
sorted_list = sorted(my_list)
print(sorted_list)
```

7. **Generate n numbers between 0 and 1 exclusively.**

```python
import random
n = 10
random_numbers = [random.random() for _ in range(n)]
print(random_numbers)
```

8. **Generate n numbers between 0 and 1 inclusively.**

```python
import random
n = 11
random_numbers = [random.random() for _ in range(n)]
print(random_numbers)
```

9. **Create an identity matrix.**

```python
n = 5
identity_matrix = [[1 if i == j else 0 for i in range(n)] for j in range(n)]
print(identity_matrix)
```

10. **Create an X matrix.**

```python
n = 5
x_matrix = [[1 if i == j or i + j == n - 1 else 0 for i in range(n)] for j in range(n)]
print(x_matrix)
```

11. **Generate a zero matrix with the same shape as another.**

```python
# Assuming `another` is a 2D list with dimensions (m, n)
m, n = len(another), len(another[0])
zero_matrix = [[0] * n for _ in range(m)]
print(zero_matrix)
```

12. **Check if an array is a palindrome.**

```python
def is_palindrome(array):
    return array == array[::-1]

array = [1, 2, 3, 2, 1]
result = is_palindrome(array)
print(result)
```

13. **Convert a number to a string.**

```python
number = 17
number_str = str(number)
print(number_str)
```

14. **Convert a string to a number.**

```python
number_str = "42"
number = int(number_str)
print(number)
```

15. **Convert a string to a list of code points.**

```python
string = "Uiua"
code_points = [ord(char) for char in string]
print(code_points)
```

16. **Convert a list of code points to a string.**

```python
code_points = [85, 105, 117, 97]
string = "".join([chr(code) for code in code_points])
print(string)
```

17. **Parse a string as a base 2 number.**

```python
binary_str = "110"
decimal_number = int(binary_str, 2)
print(decimal_number)
```

18. **Parse a string as a base 3 (X between 3 and 9) number.**

```python
base_x_str = "210"
base_x = int(base_x_str, 3)
print(base_x)
```

19. **Identify the indices of all 1s.**

```python
array = [0, 1, 0, 0, 1]
indices_of_ones = [i for i, value in enumerate(array) if value == 1]
print(indices_of_ones)
```

20. **Interleave two arrays.**

```python
array1 = [1, 2, 3]
array2 = [4, 5, 6]
interleaved_array = [x for pair in zip(array1, array2) for x in pair]
print(interleaved_array)
```

21. **Intersperse an item between the rows of an array.**

```python
array = [[1, 2], [3, 4], [5, 6]]
item = [0, 0]
interspersed_array = [val for pair in zip(array, [item] * len(array)) for val in pair]
print(interspersed_array)
```

22. **Split an array by a delimiter.**

```python
array = [1, 2, 3, 4, 5]
delimiter = 3
split_array = [[]]
for item in array:
    if item == delimiter:
        split_array.append([])
    else:
        split_array[-1].append(item)
print(split_array)
```

23. **Split an array by a delimiter with fill elements.**

```python
array = [1, 2, 3, 4, 5]
delimiter = 3
fill_element = 0
split_array = []
temp = []
for item in array:
    if item == delimiter:
        split_array.append(temp)
        temp = []
    else:
        temp.append(item)
split_array.append(temp + [fill_element])
print(split_array)
```

24. **Split an array by a delimiter while keeping empty segments.**

```python
array = [1, 3, 3, 4, 5, 3, 3, 2]
delimiter = 3
split_array = []
temp = []
for item in array:
    if item == delimiter:
        split_array.append(temp)
        temp = []
    else:
        temp.append(item)
split_array.append(temp)
print(split_array)
```

25. **Split an array into groups of contiguous equal elements.**

```python
from itertools import groupby

array = [1, 1, 1, 2, 2, 1, 1, 4]
grouped_array = [list(group) for key, group in groupby(array)]
print(grouped_array)
```

26. **Find the nth Fibonacci number.**

```python
def fibonacci(n):
    a, b = 0, 1
    for _ in range(n - 1):
        a, b = b, a + b
    return b

n = 10
fib_number = fibonacci(n)
print(fib_number)
```

27. **Remove all instances of an element from a list.**

```python
element = 4
my_list = [1, 4, 2, 0, 5, 4, 3]
filtered_list = [item for item in my_list if item != element]
print(filtered_list)
```

28. **Remove all instances of a row from an array.**

```python
row_to_remove = [2, 0, 5, 3]
array = [[1, 4, 2, 0], [5, 3, 2, 0], [2, 0, 5, 3]]
filtered_array = [row for row in array if row != row_to_remove]
print(filtered_array)
```

29. **Filter elements based on a fixed predicate.**

```python
my_list = [0, 2, 3, 4, 0, 5]
filtered_list = [item for item in my_list if item == 0]
print(filtered_list)
```

30. **Filter elements based on a dynamic predicate.**

```python
def dynamic_predicate(item):
    return item % 2 == 0

my_list = [1, 2, 3, 4, 5, 6]
filtered_list = [item for item in my_list if dynamic_predicate(item)]
print(filtered_list)
```

31. **Find the most common row in an array.**

```python
from collections import Counter

array = [[1, 2, 3], [3, 4, 5], [1, 2, 3], [4, 5, 6]]
most_common_row = max(array, key=array.count)
print(most_common_row)
```

32. **Convert a string to uppercase.**

```python
string = "These are Words"
uppercase_string = string.upper()
print(uppercase_string)
```

33. **Convert a string to lowercase.**

```python
string = "These are Words"
lowercase_string = string.lower()
print(lowercase_string)
```

34. **Check if a string exists in a list of strings.**

```python
search_string = "uiua"
list_of_strings = ["apl", "bqn", "uiua"]
string_exists = search_string in list_of_strings
print(string_exists)
```

35. **Trim leading whitespace.**

```python
string = "   ← remove these"
trimmed_string = string.lstrip()
print(trimmed_string)
```

36. **Trim trailing whitespace.**

```python
string = "remove these →   "
trimmed_string = string.rstrip()
print(trimmed_string)
```

37. **Trim prefix matching characters from a set.**

```python
string = "ccab ← remove this"
prefix_to_remove = "abc"
trimmed_string = string.lstrip(prefix_to_remove)
print(trimmed_string)
```

38. **Trim suffix matching characters from a set.**

```python
string = "remove this → bcaa"
suffix_to_remove = "abc"
trimmed_string = string.rstrip(suffix_to_remove)
print(trimmed_string)
```

39. **Trim whitespace.**

```python
string = "  abc xyz   "
trimmed_string = string.strip()
print(trimmed_string)
```

40. **Upscale a 2D matrix.**

```python
matrix = [[0, 1], [1, 0]]
upscaled_matrix = [[val for val in row] for row in matrix]
upscaled_matrix = [[elem for row in upscaled_matrix] for elem in matrix[0]]
print(upscaled_matrix)
```

41. **Upscale a colored image.**

```python
# Assuming you have an image represented as a 2D list of RGB values
image = [[(0, 0, 255), (0, 255, 0)], [(255, 0, 0), (0, 0, 0)]]
upscaled_image = [[pixel for pixel in row] for row in image]
upscaled_image = [[px for row in upscaled_image] for px in image[0]]
print(upscaled_image)
```

42. **Perform linear interpolation between two values.**

```python
value1 = 0
value2 = 10
alpha = 0.2
interpolated_value = value1 * (1 - alpha) + value2 * alpha
print(interpolated_value)
```

43. **Set the value of an array at a specific index.**

```python
my_list = [1, 2, 3, 4, 5]
index = 2
new_value = 10
my_list[index] = new_value
print(my_list)
```

44. **Create a matrix filled with random 0s or 1s.**

```python
import random

n = 5
random_matrix = [[random.choice([0, 1]) for _ in range(n)] for _ in range(n)]
print(random_matrix)
```

45. **Create a matrix filled with random numbers.**

```python
import random

n = 5
random_matrix = [[random.uniform(0, 1) for _ in range(n)] for _ in range(n)]
print(random_matrix)
```

46. **Calculate the arithmetic mean.**

```python
numbers = [85, 105, 117, 97]
mean = sum(numbers) / len(numbers)
print(mean)
```

47. **Calculate the dot product.**

```python
vector1 = [1, 2, 3]
vector2 = [4, -5, 6]
dot_product = sum(v1 * v2 for v1, v2 in zip(vector1, vector2))
print(dot_product)
```

48. **Calculate the cross product.**

```python
vector1 = (1, 2, 3)
vector2 = (4, 5, 6)
cross_product = (
    vector1[1] * vector2[2] - vector1[2] * vector2[1],
    vector1[2] * vector2[0] - vector1[0] * vector2[2],
    vector1[0] * vector2[1] - vector1[1] * vector2[0],
)
print(cross_product)
```

49. **Perform matrix multiplication.**

```python
matrix1 = [[1, 2], [3, 4]]
matrix2 = [[5, 6], [7, 8]]
result_matrix = [[sum(a * b for a, b in zip(row1, col2)) for col2 in zip(*matrix2)] for row1 in matrix1]
print(result_matrix)
```

50. **Calculate the matrix power (works with scalars as well).**

```python
def matrix_power(matrix, exponent):
    if exponent == 1:
        return matrix
    result = matrix
    for _ in range(1, exponent):
        result = [[sum(a * b for a, b in zip(row1, col2)) for col2 in zip(*result)] for row1 in matrix]
    return result

matrix = [[1, 2], [3, 4]]
exponent = 2
result_matrix = matrix_power(matrix, exponent)
print(result_matrix)
```

51. **Add a leading axis with a length of 1 to an array.**

```python
import numpy as np  # For creating a NumPy array, if needed

array = [1, 2, 3, 4, 5]
leading_axis_array = np.array(array)[np.newaxis]
print(leading_axis_array)
```

52. **Repeatedly apply a function and collect intermediate results into an array.**

```python
def custom_function(x):
    return x * 2

n = 10
initial_value = 1
intermediate_results = [initial_value]
for _ in range(n):
    new_result = custom_function(intermediate_results[-1])
    intermediate_results.append(new_result)
print(intermediate_results)
```

