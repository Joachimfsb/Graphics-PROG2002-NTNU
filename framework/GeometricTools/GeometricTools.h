#ifndef GEOMETRICTOOLS_H
#define GEOMETRICTOOLS_H

#include <array>
#include <vector>
#include <cmath>

namespace Framework {
  namespace GeometricTools {
    
    using uint = unsigned int; //sets uint to unsigned int

    /**
     *  Geometry (vertecies) of unit trinagle in 2D.
     * 
     *  @returns Array of vertecies (float)
     */
    constexpr std::array<float, 3*2> UnitTriangle2D = { //(x,y)
        -0.5f, -0.5f,  //bottom-left
        0.5f, -0.5f,  //bottom-right
        0.0f,  0.5f   //top-center
      };

    /**
     *  Geometry (vertecies) of unit square in 2D.
     * 
     *  @returns Array of vertecies (float)
     */
    constexpr std::array<float, 4*2> UnitSquare2D = { //(x,y)
        -0.5f, -0.5f, //bottom-left
        0.5f, -0.5f, //bottom-right
        0.5f,  0.5f, //top-right
        -0.5f,  0.5f  //top-left
      };
    
    /**
     *  Geometry (vertices) of unit grid in 2D
     * 
     *  If template argument 'colors' is true, a color attribute is added to the vertex buffer.
     * 
     *  If template argument 'textures' is true, a texture coordinate attribute is added to the vertex buffer.
     * 
     *  @returns
     * Vector of vertices (floats) with format:
     * 
     * 1. positions (2)
     * 
     * 2. (opt) colors (4)
     * 
     * 3. (opt) texture coordinates (2)
     */
    template <uint rows, uint cols, bool colors = false, bool textures = false>
    constexpr std::vector<float> UnitGridGeometry2D() {
      std::vector<float> vertecies;

      if (rows == 0 || cols == 0) return vertecies; // Cannot have 0 rows or cols

      // Loop through and create each vertex
      bool even = true;
      for (int y = 0; y <= rows; y++) {
        for (int x = 0; x <= cols; x++) {
          // Position attribute
          vertecies.push_back((-1)+(float)x*2/cols); // Vertex x-coord
          vertecies.push_back((-1)+(float)y*2/rows); // Vertex y-coord
          //std::cout << "(" << x << "x," << y << "y) ";

          // Color attribute
          if (colors) {
            if (even) {
              vertecies.insert(vertecies.end(), {1.0f, 1.0f, 1.0f, 1.0f});
            } else {
              vertecies.insert(vertecies.end(), {0.0f, 0.0f, 0.0f, 1.0f});
            }
          }

          // Textures attribute
          if (textures) {
            vertecies.push_back((float)x/cols);
            vertecies.push_back((float)y/rows);
          }

          even = !even;
        }
        //std::cout << "\n";
      }
      return vertecies;
    }
    
    /**
     *  Topology (indecies) of unit grid in 2D (using triangles).
     * 
     *  @returns Array of indecies (uint)
     */
    template <uint rows, uint cols>
    constexpr std::vector<uint> UnitGridTopologyTriangles() {
      std::vector<uint> indecies;
      if (rows == 0 || cols == 0) return indecies; // Cannot have 0 rows or cols
      
      // Loop through each row and column except the last one
      auto rowMult = cols+1; // Multiplier for row number / amount of columns+1
      /*
      vertexArray = [punkt1, punkt2, punkt3,
                    punkt4, punkt5, punkt6,
                    punkt7, punkt8, punkt9] 
      cols = 2
      rows = 2
      rowMult = 3
      */

      for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
          
          // Single tile (square)
          // Primitive 1 (triangle)
          //std::cout << "(";
          indecies.push_back(rowMult* row   + col);   // std::cout << indecies.back() << ",";  //vertex a (top-left)
          indecies.push_back(rowMult* row   +(col+1));// std::cout << indecies.back() << ",";  //vertex b (top-right)
          indecies.push_back(rowMult*(row+1)+(col+1));// std::cout << indecies.back() << ") "; //vertex c (bottom-right)
          // Primitive 2 (triangle)
          //std::cout << "(";
          indecies.push_back(rowMult* row   + col);   // std::cout << indecies.back() << ",";   //vertex a (top-left)
          indecies.push_back(rowMult*(row+1)+ col);   // std::cout << indecies.back() << ",";   //vertex d (bottom-left)
          indecies.push_back(rowMult*(row+1)+(col+1));// std::cout << indecies.back() << ")\n"; //vertex c (bottom-right)
        }
      }

      return indecies;
    }



    /**
     *  Geometry (vertices) of unit cube in 3D
     * 
     *  @returns Vector of vertices (3 floats per vertex)
     */
    constexpr std::array<float, 8*3> UnitCubeGeometry3D = {
      -0.5, -0.5, -0.5, // A 0
      0.5, -0.5, -0.5, // B 1
      0.5,  0.5, -0.5, // C 2
      -0.5,  0.5, -0.5, // D 3
      -0.5, -0.5,  0.5, // E 4
      0.5, -0.5,  0.5, // F 5
      0.5,  0.5,  0.5, // G 6
      -0.5,  0.5,  0.5  // H 7
    };
    
    /**
     *  Topology (indecies) of unit cube in 2D (using triangles).
     * 
     *  @returns Array of indecies (uint)
     */
    constexpr std::array<uint, 3*2*6> UnitCubeTopology3D = {
      0, 4, 5, // AEF
      0, 5, 1, // AFB
      1, 5, 6, // BFG
      1, 6, 2, // BGC
      2, 6, 7, // CGH
      2, 7, 3, // CHD
      3, 7, 4, // DHE
      3, 4, 0, // DEA
      4, 7, 6, // EHG
      4, 6, 5, // EGF
      0, 1, 2, // ABC
      0, 2, 3  // ACD
    };


    /**
     *  Geometry (vertices) of unit cube in 3D with normals. 
     * 
     *  @returns Vector of vertices and normals (3 * 3 floats per vertex * 3 floats for single normal)
     */
    constexpr std::array<float, 3 * 8*3 * 2> UnitCube3D24WNormals = {
       /* Vertex: A 0  */ -0.5, -0.5, -0.5, /* Normal: 1 */ -1.0, 0, 0,
       /* Vertex: D 1  */ -0.5,  0.5, -0.5, /* Normal: 1 */ -1.0, 0, 0,
       /* Vertex: E 2  */ -0.5, -0.5,  0.5, /* Normal: 1 */ -1.0, 0, 0,
       /* Vertex: H 3  */ -0.5,  0.5,  0.5, /* Normal: 1 */ -1.0, 0, 0,

       /* Vertex: B 4  */  0.5, -0.5, -0.5, /* Normal: 2 */  1.0, 0, 0,
       /* Vertex: C 5  */  0.5,  0.5, -0.5, /* Normal: 2 */  1.0, 0, 0,
       /* Vertex: F 6  */  0.5, -0.5,  0.5, /* Normal: 2 */  1.0, 0, 0,
       /* Vertex: G 7  */  0.5,  0.5,  0.5, /* Normal: 2 */  1.0, 0, 0,

       /* Vertex: A 8  */ -0.5, -0.5, -0.5, /* Normal: 3 */ 0, -1.0, 0,
       /* Vertex: B 9  */  0.5, -0.5, -0.5, /* Normal: 3 */ 0, -1.0, 0,
       /* Vertex: E 10 */ -0.5, -0.5,  0.5, /* Normal: 3 */ 0, -1.0, 0,
       /* Vertex: F 11 */  0.5, -0.5,  0.5, /* Normal: 3 */ 0, -1.0, 0,

       /* Vertex: C 12 */  0.5,  0.5, -0.5, /* Normal: 4 */ 0,  1.0, 0,
       /* Vertex: D 13 */ -0.5,  0.5, -0.5, /* Normal: 4 */ 0,  1.0, 0,
       /* Vertex: G 14 */  0.5,  0.5,  0.5, /* Normal: 4 */ 0,  1.0, 0,
       /* Vertex: H 15 */ -0.5,  0.5,  0.5, /* Normal: 4 */ 0,  1.0, 0,

       /* Vertex: A 16 */ -0.5, -0.5, -0.5, /* Normal: 5 */ 0, 0, -1.0,
       /* Vertex: B 17 */  0.5, -0.5, -0.5, /* Normal: 5 */ 0, 0, -1.0,
       /* Vertex: C 18 */  0.5,  0.5, -0.5, /* Normal: 5 */ 0, 0, -1.0,
       /* Vertex: D 19 */ -0.5,  0.5, -0.5, /* Normal: 5 */ 0, 0, -1.0,

       /* Vertex: E 20 */ -0.5, -0.5,  0.5, /* Normal: 6 */ 0, 0, 1.0,
       /* Vertex: F 21 */  0.5, -0.5,  0.5, /* Normal: 6 */ 0, 0, 1.0,
       /* Vertex: G 22 */  0.5,  0.5,  0.5, /* Normal: 6 */ 0, 0, 1.0,
       /* Vertex: H 23 */ -0.5,  0.5,  0.5, /* Normal: 6 */ 0, 0, 1.0
    };
    
    /**
     *  Topology (indecies) of unit cube in 2D (using triangles) when geometry includes normals.
     * 
     *  @returns Array of indecies (uint)
     */
    constexpr std::array<uint, 3*2*6> UnitCube3DTopologyTriangles24 = {
      // Normal 1
      1, 0, 2, // DAE
      1, 2, 3, // DEH
      // Normal 2
      4, 5, 7, // BCG
      4, 7, 6, // BGF
      // Normal 3
      8, 9, 11, // ABF
      8, 11, 10, // AFE
      // Normal 4
      12, 13, 15, // CDH
      12, 15, 14, // CHG
      // Normal 5
      17, 16, 19, // BAD
      17, 19, 18, // BDC
      // Normal 6
      22, 23, 20, // GHE
      22, 20, 21  // GEF
    };
  };
};
#endif