// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 156.

// Package geometry defines simple types for plane geometry.
//!+point
package main

import(
	"fmt"
	"math"
	"math/rand"
	"os"
	"strconv"
	"time"
) 

type Point struct{ x, y float64 }

func (point Point) X() float64 {
	return point.x
}

func (point Point) Y() float64 {
	return point.y
}

// traditional function
func Distance(p, q Point) float64 {
	return math.Hypot(q.x-p.x, q.y-p.y)
}

// same thing, but as a method of the Point type
func (p Point) Distance(q Point) float64 {
	return math.Hypot(q.x-p.x, q.y-p.y)
}

func onSegment(p, q, r Point) bool {
	return q.x <= math.Max(p.x, r.x) && q.x >= math.Min(p.x, r.x) && 
	q.y <= math.Max(p.y, r.y) && q.y >= math.Min(p.y, r.y)
}

func orientation(p, q, r Point) int {
	val := (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y)
	if val == 0 {
		return 0
	}
	if val > 0 {
		return 1
	} else {
		return 2
	}
}

func doIntersect(p1, p2, q1, q2 Point) bool {
	o1 := orientation(p1, p2, q1)
	o2 := orientation(p1, p2, q2)
	o3 := orientation(q1, q2, p1)
	o4 := orientation(q1, q2, p2)

	if o1 != o2 && o3 != o4 { return true }

	if o1 == 0 && onSegment(p1, q1, p2) { return true }
	if o2 == 0 && onSegment(p1, q2, p2) { return true }
	if o3 == 0 && onSegment(q1, p1, q2) { return true }
	if o4 == 0 && onSegment(q1, p2, q2) { return true }

	return false

}

//!-point

//!+path

// A Path is a journey connecting the points with straight lines.
type Path []Point

// Distance returns the distance traveled along the path.
func (path Path) Distance() float64 {
	sum := 0.0
	for i := range path {
		if i > 0 {
			sum += path[i-1].Distance(path[i])
		}
	}
	return sum
}

func generateRandom(min, max float64) float64 {
	return min + rand.Float64() * (max + 1 - min)
}

func main() {
	sides, err := strconv.Atoi(os.Args[1])
	if err != nil {
		fmt.Println(err)
	}

	if sides <= 2 {
		fmt.Println("Error: enter a valid set of parameters")	
		fmt.Println("Usage: geometry <3 or more sides>")
		return
	}

	points := make(Path, sides) 

	intersect := false
	
	fmt.Printf("Generating a [%d] sides figure\n", sides)

	for{
		rand.Seed(time.Now().UnixNano())

		for i := 0; i < sides; i++{
			points[i] = Point{generateRandom(-100.0,100.0),generateRandom(-100.0,100.0)}
		}

		for i := 0; i < sides; i++{
			if doIntersect(points[i%sides],points[(1+i)%sides],points[(2+i)%sides],points[(3+i)%sides]) {
				intersect = true
				break
			}
		}

		if !intersect {
			fmt.Println("Figure's vertices")
			for i := range points {
				fmt.Printf("\t( %6.2f, %6.2f)\n", points[i].X(), points[i].Y())
			}
			fmt.Println("Figure's perimeter: ")
			fmt.Printf("\t %0.2f\n", points.Distance())
			return
		}else{
			intersect = false
		}

	}
}

//!-path
