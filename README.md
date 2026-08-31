Coding Challenge solutions
# Solution
## Understanding
understanding the problem
Task 0
So this is relatively straightforward, I have to create the copy of the repo in my account and make it public.

Task 1
Slightly tricky to understand, but the gist of it all is that there is a data stream that has to be encoded so that it can be transmitted over wires and get decoded at the other end. What this is trying to achieve is that in serial communication there are delimiters in the data (such as 0x00) and the receiver could mistake any data that involves that byte, therefore it is encoded and decoded.

In this task, the code has a couple of errors with the frame encode and frame decode functions. The frame encode will basically scan the data for 0x00 bytes and replace them with pointer numbers that say to jump a few bytes ahead to find the zero. This makes sure that there aren't any zeros within it. The frame decode will reverse the process that frame encode did.

Starting off with the first error, there was an undeclared variable and the code segment didn't follow the specific norms of COBS (Consistent Overhead Byte Stuffing) so a couple of changes were made to it.

The second problem had problems with the looping structure, and the iteration for the loop, and once it does reach the final byte, no operation is performed whatsoever, so this fix will sort it out.

The third problem was with the decoder and the for loop. It also had to be fixed in accordance to COBS.

Task 2
So this is a bit hard to understand, so I'll try to make sense of it. There are threads on a CPU and it can perform a single task; with multiple CPU cores, you can perform more tasks simultaneously. Broadly, software components run on two types of threads in this setup: producer threads and consumer threads.

Now in this specific task, I could find two problems. One is with the reader-writer lock. There can be multiple readers reading at the same time, but the first reader must lock the resource to prevent writers from entering and corrupting data while reading is active.

In the second problem, the producer function message_queue_push was completely empty, so no data was being added to the queue. As a result, the pop function was waiting forever for data to arrive, freezing the program entirely.

## Thought Process
After understanding the problem, describe how you decided to proceed towards solving the question. Also document any use of external resources or AI tools. 

## Implementation
How did you decide to implement your solution.


