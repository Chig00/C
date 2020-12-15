#include <stdio.h>

#define PI 3.14159265359

double circum_calc(double radius) {
  double circumference = 2 * PI * radius;
  return circumference;
}

double area_calc(double radius) {
  double area = PI * radius * radius;
  return area;
}

int main() {
  double radius;
  printf("Radius: ");
  scanf("%lf", &radius);
  
  double circumference = circum_calc(radius);
  double area = area_calc(radius);
  
  printf("Circumference: %lf\n", circumference);
  printf("Area:          %lf\n", area);
          
  return 0;
}