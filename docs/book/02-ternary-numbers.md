# 02 – Ternary Numbers 101 (Balanced Ternary)

## In short
We’ll use balanced ternary: each place holds a trit value from {−1, 0, +1}. We’ll write them as −, 0, + for short. Example: “+0−” means (+1)·3² + 0·3¹ + (−1)·3⁰ = 9 − 1 = 8.

## What does this mean?

Okay, let's start from the beginning.
Since the dawn of modern computing, we use two values. The integers 0 and 1. And we use it for a special reason.
A electirc circuit make use of two different states at the first glance. 0 and 1 represent if there is any meassurable voltage or not, on and off, light and no light.
This is why a binary system took place in modern computation.
But it have some limitation to overcome. Let's say we want to show a negative number. We need to save one bit ,what means the represenation of one single digit - 0 or 1 to determine if its a negative of positive number.
This is why modern programming langueas have signed and unsigned intergeres or floats.
And in 4 digits you can only represent a maxmimal value of 255 when it is show as a decimal number.

It is known that in russia there where scientists who was aware of this flaw and created ternary systems. But it is a fact, that a country that relays on socialism,
have major issues when it comes to the rules of market, a thing that we call capitalism in the western world. And, not to forget, russia also had and has some problems when it comes to human rights what makes it also difficult to get a good product to the market. So this techniqe was buried by the sands of time and because of weak security measurments when it comes to balconies and freedom of speach. Anyways...

A ternary system have his very own advantages. Instead of 0 and 1 we can make use of -1, 0, and 1. You might think, this is just one Value more, but with this little concept we can higher the capacity of a single bit, can do more non-linar decissions and we can show negative numbers for free.
You might also wondering, how we can make use of -1 in a system that only know on and off. This is quiet easy - we don't have to. Since a transistor have one single input - with the state on or off - a ternary transistor has two of them after my design idea. The original Setun (the russian ternary Computer) made use of Voltage Steps, what is in my mind not nesecarry and to complex to ceate reliable microchips. In my design, as said, I propose two datalines per transistor One to show if it is not 0 - or off - and one to distinct if it is -1 or 1. Et voila' - we have create a ternary transistor with three states by just having the values 1 and 0.

Now we can do a lot of math and logical things, we could not before - or let's say, we could but with a lot of downsizes.

## The math behind ternary - or - how we can use it as decimal thinking entities

A binary calculation is quiet easy and can be performed real quick, without make use of power calculations.
You can make up a sheet with the following numbers:

`1   2   4   8   16  32  ...`

Now let's say, we have binary number like 010010. When we take every bit (the 0 or 1) and write it directly underneath the decimal numbers, we can easily calculate the number.

```
1   2   4   8   16  32
0   1   0   0   1   0
------------------------ 
    2+          16+          = 18
```

This is dead simple, but what about representing nagative numbers? You cannot unless you add a sign somewhere to determine if it's a negative or positive number.
Surprisingly you can make use of the same system to calculate number and got some nice extras.
As we saw on the binary numbers, we can use also a decimal sheet for the calcumation, but instead of the power of two, we make use of power of three.

`1   3   9   27 ...` 
 
Now lets say, we have a ternary number like 1(-1)1. This number is pretty complex to read. So we rather should go to another representation. 
From this point of the book I will write -1 as T, so 1T1 reads a bit better. We just going to replace -1 with a T do show that we left binary system and make use of ternary system. 
There might be another case as well when we have a number 101 what would be a decimal value of 5 from ther binary perspective and a 10 from ternary. But this will be solved sooner or later.

But let's get back to the transformation between ternary and decimal. The number 1T1 can be easily calculated with the same pattern as we did on binary.
```
1       3       9       27
1       T       1       0
---------------------------
1+      (-3)+   9+            = 7
```
  
So we can read and calculate the numer 1T1 as 1+(-1)*3+1*9 or more simple 1+(-3)+9. When you followed this so far you will realise that we have a very space for numbers.
In a 8 bit binary representation we can only show 0 to 255 while 8 trit (what is the name for a ternary "bit") we have -3280 to 3280. This is a range of 6561 possible values and over 12 times more dense than a binary ever could be.
 
And with help of a simple TXOR we can make a positive trit to a negative and vince versa and can do substractions with the adder. So substractions comes for free at last.

## Why balanced?
Wouldn't it be easier to make use of unbalanced ternary numbers? I guess not.
For sure, It would be nice to have a ternary system that consist of 0, 1 and 2. But this would come at its costs.
First of all, you could not show negative numbers. You must have a extra trit that act as boolean to determine if the number is negative or positive.

Also thanks to the symetry we have a natural consitency to add and substract number. It is pretty easy to make substraction because every number has it's very own oposite number already build in.

Let me give you an example:
Let's say we have the number 1011 what is 37 in decimal. The negative number is simply T0TT (or -1 0 -1 -1). It is a natural mirror. Mathematics starts to getting real simple, that you start wondering why we suffer on Binary or Decimal notations. This advantages show up why ternary computing makes more sense than binary computing and why we should make use of balanced ternary instead of trying to create a unbalanced ternary system.  

## So we can make the following conclusions

Why balanced ternary:
- Natural sign: negative values don’t need a separate sign bit
- Symmetry: adding and subtracting feel consistent
- Simpler logic for sign/compare operations

Converting integers → balanced ternary (intuition):
- Divide by 3, but allow remainders −1, 0, +1
- “Carry” can be negative or positive


If you dare, you can try to make use of the Trit Libray and do some experiments with the TritWords.

In code, we expose `TritWord` which converts both ways:
- `TritWord.from_int(value, size=6)`
- `TritWord(...).to_int()`

Try it in Python: create a few `TritWord`s and print them; you’ll see sequences of −/0/+.
