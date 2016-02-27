# Animated-Gas-Giants

Some pseudo code for when I get home:

to calculate perlin noise:
  xlerp1 = lerp dot(grad_at_topleft, vec_from_topleft), dot(grad_at_topright, vec_from_topright), xpos  
  xlerp2 = lerp dot(grad_at_botleft, vec_from_botleft), dot(grad_at_botright, vec_from_botright), xpos  
  finlerp = lerp xlerp1, xlerp2, ypos  
  
lerp(a, b, t):  
  t3 = t*t*t  
  t4 = t3*t  
  t5 = t4*t  
  t3m = (1-t)*(1-t)*(1-t)  
  t4m = t3m*(1-t)  
  t5m = t4m*(1-t)  
  return a*(6*t5 - 15*t4 + 10*t3) + b*(6*t5m - 15*t4m + 10*t3m)  
