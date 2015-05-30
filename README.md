# Optical-Flow-Computation

This project presents a unique approach to compute the optical flow by integrating FAST ( Features from Accelerated Segment Test<sup> 1</sup> ) corner detection algorithm followed by SAD ( Sum of Absolute Differences ) based motion estimation algorithm.

In motion estimation using SAD based block matching algorithm, successive frames of the video are divided into blocks of predetermined sizes and then each block in the current frame is compared to the respective search area in the previous frame to calculate the corresponding block. This is a very compute intensive approach and might provide motion estimation values of background creating ambiguity in the results.

This project uses FAST corner detection algorithm to select the areas of interest in the image and Block Matching is then only performed on blocks containing these corner points. This provides us with motion vectors around the object in the foreground and thus reducing the block matching computation.

Code for both SAD based block matching and Optical Flow computation is provided in the src.

<sup>1 </sup>*E. Rosten, R. Porter and T. Drummond, "Faster and Better: A Machine Learning Approach to Corner Detection," Pattern Analysis and Machine Intelligence, IEEE Transactions on , vol.32, no.1, pp.105,119, January 2010.*
