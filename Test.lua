-- Variable declaration and assignment
local x = 10
local y = 20

-- Arithmetic operations
local sum = x + y
local difference = x - y
local product = x * y
local quotient = x / y
local remainder = x % y
local power = x ^ 2

-- Conditional statement
if x > y then
    print("x is greater than y")
elseif x < y then
    print("x is less than y")
else
    print("x is equal to y")
end

-- Looping
for i = 1, 5 do
    print("Loop iteration: " .. i)
end

-- Function declaration
local function add(a, b)
    return a + b
end

-- Function call
local result = add(5, 3)
print("Result of addition: " .. result)

-- Table declaration and manipulation
local myTable = { a = 1, b = 2, c = 3, ["foo"] = "foo" }
print("Value of 'b' in myTable: " .. myTable.b)
print("Length of myTable: " .. #myTable)

-- String manipulation
local str1 = "Hello"
local str2 = "world!"
local concatStr = str1 .. ", " .. str2
print(concatStr)

-- Array declaration and manipulation
local myArray = { 1, 2, 3, 4, 5 }
print("Third element of myArray: " .. myArray[3])

-- Using nil
local z
print("Value of z: " .. tostring(z))

-- Using boolean values
local isTrue = true
local isFalse = false
print("isTrue: " .. tostring(isTrue))
print("isFalse: " .. tostring(isFalse))

-- Using 'not' operator
local notTrue = not isTrue
print("notTrue: " .. tostring(notTrue))

-- Using 'and' operator
local andResult = isTrue and isFalse
print("andResult: " .. tostring(andResult))

-- Using 'or' operator
local orResult = isTrue or isFalse
print("orResult: " .. tostring(orResult))

-- Using '==' and '~=' operators
local isEqual = x == y
print("isEqual: " .. tostring(isEqual))

local isNotEqual = x ~= y
print("isNotEqual: " .. tostring(isNotEqual))

-- Using a comment
-- This is a comment.

-- Using a long string
local longString = [[
This is a long string.
It spans multiple lines.
]]
print(longString)

-- Using a single-line comment
print("Hello") -- This is a comment.

-- Using a multi-line comment
--[[
This is a
multi-line
comment.
]]

-- Using a do...end block
do
    print("Inside do...end block")
end

-- Using a repeat...until loop
local i = 1
repeat
    print("Repeat...until loop iteration: " .. i)
    i = i + 1
until i > 5

-- Using a while loop
local j = 1
while j <= 5 do
    print("While loop iteration: " .. j)
    j = j + 1
end

-- Using a goto statement
local k = 1
::start::
if k > 10 then
    goto finish
end
print("Goto statement iteration: " .. k)
k = k + 1
goto start
::finish::

-- Using a break statement
for m = 1, 10 do
    if m > 5 then
        break
    end
    print("Break statement iteration: " .. m)
end

-- Using a return statement
local function testReturn()
    return "Returned value"
end
print(testReturn())

-- Using a local function declaration
local function outerFunction()
    local function innerFunction()
        print("Inside inner function")
    end
    innerFunction()
end
outerFunction()

-- Using a global variable
globalVar = 100
print("Value of globalVar: " .. globalVar)

-- Using a variable scope
do
    local scopedVar = "Scoped variable"
    print(scopedVar)
end
-- print(scopedVar) -- This line would cause an error because scopedVar is out of scope

-- Using a numerical for loop with step
for n = 10, 1, -2 do
    print("Numerical for loop with step iteration: " .. n)
end

-- Using a numerical for loop with step and floating-point values
for p = 0, 1, 0.1 do
    print("Numerical for loop with step and floating-point iteration: " .. p)
end

-- Using a continue statement (via label and goto)
local continueFlag = false
for q = 1, 10 do
    if continueFlag then
        goto continue
    end
    if q == 5 then
        continueFlag = true
        goto continue
    end
    print("Continue statement iteration: " .. q)
    ::continue::
end

a = [====[

]====]

-- Using concatenation assignment operator
local concatAssignment = "Hello"
concatAssignment = concatAssignment .. " world!"
print(concatAssignment)

-- Using length operator
local strLength = #concatAssignment
print("Length of concatAssignment: " .. strLength)

-- Using a ternary operator
local ternaryVar = x > y and "x is greater than y" or "x is not greater than y"
print(ternaryVar)

-- Using bit operators
local bitNot = ~0
local bitOr = 0 | 1
local bitAnd = 1 & 1
local bitShiftLeft = 1 << 1
local bitShiftRight = 8 >> 1

-- valid numbers
local validNum1 = 3
local validNum2 = 345
local validNum3 = 0xff
local validNum4 = 0xBEBADA

-- valid floats
local validFloat1 = 3.0
local validFloat2 = 3.1416
local validFloat3 = 314.16e-2
local validFloat4 = 04.31416E1
local validFloat5 = 34e1
local validFloat6 = 0x0.1E
local validFloat7 = 0xA23p-4
local validFloat8 = 0X1.921FB54442D18P+1

local multilineString = [===[test 
multilineString]===]

local func = (function()
    print("hi")
end)
