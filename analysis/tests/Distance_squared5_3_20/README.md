Change from OGMTests1_3_20 is from distance() function to distance_squared() function. The idea being that there is no need to find the square root of distance, so we can make comparisions based on distance squared, since the sqrt() function is computationally intensive.

commit:
c3402bfefce4b792504ff8037584849a0396ea27