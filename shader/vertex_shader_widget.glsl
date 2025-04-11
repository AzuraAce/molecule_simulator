#version 330 core
layout(location = 0) in vec2 aPos;

uniform vec2 u_resolution;
uniform vec2 u_position;  // Bottom-left corner in screen space
uniform vec2 u_size;      // Size of the button (width, height)

void main() {
    // Step 1: Scale the input position by the size of the button (width, height)
    vec2 pos = aPos * u_size + u_position; // Local space to screen space
    
    // Step 2: Normalize the position from screen space to NDC
    vec2 zeroToOne = pos / u_resolution;       // Normalize from [0, resolution] to [0, 1]
    vec2 zeroToTwo = zeroToOne * 2.0;          // Scale from [0, 1] to [0, 2]
    vec2 clipSpace = zeroToTwo - 1.0;          // Map from [0, 2] to [-1, 1] (OpenGL NDC)

    // Step 3: Apply the correct coordinate system for OpenGL (flip the Y-axis)
    gl_Position = vec4(clipSpace.x, clipSpace.y * -1.0, 0.0, 1.0); // Flip Y-axis for OpenGL NDC
}

