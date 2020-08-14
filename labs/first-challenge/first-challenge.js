const getLength = (a) => {
    counter = 0
    for( let item of a){
        if(Array.isArray(item)) counter += getLength(item)
        else counter +=1
    }
    return counter
}

console.log("test case 1:", getLength([1, [2, 3]]))
console.log("test case 2:", getLength([1, [2, [3, 4]]]))
console.log("test case 3:", getLength([1, [2, [3, [4, [5, 6]]]]]))
console.log("test case 4:", getLength([1, [2], 1, [2], 1]))


