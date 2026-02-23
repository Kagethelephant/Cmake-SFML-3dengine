#pragma once
#include <glad/glad.h>

//--------------------------------------------
// Viewport RAII
//--------------------------------------------
struct GLScopedViewport {
    GLint oldViewport[4];
    GLScopedViewport(GLint x, GLint y, GLsizei w, GLsizei h) {
        glGetIntegerv(GL_VIEWPORT, oldViewport);
        glViewport(x, y, w, h);
    }

    // Non-copyable
    GLScopedViewport(const GLScopedViewport&) = delete;
    GLScopedViewport& operator=(const GLScopedViewport&) = delete;

    ~GLScopedViewport() { glViewport(oldViewport[0], oldViewport[1], oldViewport[2], oldViewport[3]); }
};

//--------------------------------------------
// VAO RAII
//--------------------------------------------
struct GLScopedVAO {
    GLint oldVAO;
    GLScopedVAO(GLuint vao) {
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &oldVAO);
        glBindVertexArray(vao);
    }

    // Non-copyable
    GLScopedVAO(const GLScopedVAO&) = delete;
    GLScopedVAO& operator=(const GLScopedVAO&) = delete;

    ~GLScopedVAO() { glBindVertexArray(oldVAO); }
};

//--------------------------------------------
// VBO RAII
//--------------------------------------------
struct GLScopedVBO {
    GLint oldVBO;
    GLScopedVBO(GLuint vbo) {
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &oldVBO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
    }

    // Non-copyable
    GLScopedVBO(const GLScopedVBO&) = delete;
    GLScopedVBO& operator=(const GLScopedVBO&) = delete;

    ~GLScopedVBO() { glBindBuffer(GL_ARRAY_BUFFER, oldVBO); }
};

//--------------------------------------------
// EBO RAII (element buffer)
//--------------------------------------------
struct GLScopedEBO {
    GLint oldEBO;
    GLScopedEBO(GLuint ebo) {
        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &oldEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    }
   
    // Non-copyable
    GLScopedEBO(const GLScopedEBO&) = delete;
    GLScopedEBO& operator=(const GLScopedEBO&) = delete;

    ~GLScopedEBO() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, oldEBO); }
};


//--------------------------------------------
// FBO RAII
//--------------------------------------------
struct GLScopedFBO {
    GLint oldFBO;
    GLScopedFBO(GLuint fbo) {
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    }

    // Non-copyable
    GLScopedFBO(const GLScopedFBO&) = delete;
    GLScopedFBO& operator=(const GLScopedFBO&) = delete;

    ~GLScopedFBO() { glBindFramebuffer(GL_FRAMEBUFFER, oldFBO); }
};

//--------------------------------------------
// Cull face RAII
//--------------------------------------------
struct GLScopedCullFace {
    GLboolean oldEnabled;
    GLint oldMode;
    GLScopedCullFace(GLenum mode) {
        oldEnabled = glIsEnabled(GL_CULL_FACE);
        glGetIntegerv(GL_CULL_FACE_MODE, &oldMode);
        glEnable(GL_CULL_FACE);
        glCullFace(mode);
    }

    // Non-copyable
    GLScopedCullFace(const GLScopedCullFace&) = delete;
    GLScopedCullFace& operator=(const GLScopedCullFace&) = delete;

    ~GLScopedCullFace() {
        if (!oldEnabled) glDisable(GL_CULL_FACE);
        else glCullFace(oldMode);
    }
};

//--------------------------------------------
// Depth test RAII
//--------------------------------------------
struct GLScopedDepthTest {
    GLboolean oldEnabled;
    GLScopedDepthTest(bool enable = true) {
        oldEnabled = glIsEnabled(GL_DEPTH_TEST);
        if (enable) glEnable(GL_DEPTH_TEST);
        else glDisable(GL_DEPTH_TEST);
    }
    // Non-copyable
    GLScopedDepthTest(const GLScopedDepthTest&) = delete;
    GLScopedDepthTest& operator=(const GLScopedDepthTest&) = delete;

    ~GLScopedDepthTest() {
        if (oldEnabled) glEnable(GL_DEPTH_TEST);
        else glDisable(GL_DEPTH_TEST);
    }
};

//--------------------------------------------
// Shader Program RAII
//--------------------------------------------
struct GLScopedProgram {
    GLint previous = 0;

    explicit GLScopedProgram(GLuint program) {
        glGetIntegerv(GL_CURRENT_PROGRAM, &previous);
        glUseProgram(program);
    }

    // Non-copyable
    GLScopedProgram(const GLScopedProgram&) = delete;
    GLScopedProgram& operator=(const GLScopedProgram&) = delete;

    ~GLScopedProgram() {
        glUseProgram(previous);
    }
};

//--------------------------------------------
// Shader Program RAII
//--------------------------------------------
struct GLScopedTexture2D {
    GLint previous = 0;

    explicit GLScopedTexture2D(GLuint tex) {
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &previous);
        glBindTexture(GL_TEXTURE_2D, tex);
    }

    GLScopedTexture2D(const GLScopedTexture2D&) = delete;
    GLScopedTexture2D& operator=(const GLScopedTexture2D&) = delete;

    ~GLScopedTexture2D() {
        glBindTexture(GL_TEXTURE_2D, previous);
    }
};


struct GLScopedCapability
{
   GLenum cap;
   GLboolean wasEnabled;

   GLScopedCapability(GLenum capability, bool enable) : cap(capability){
      wasEnabled = glIsEnabled(cap);
      if (enable) glEnable(cap);
      else glDisable(cap);
   }

    GLScopedCapability(const GLScopedCapability&) = delete;
    GLScopedCapability& operator=(const GLScopedCapability&) = delete;

   ~GLScopedCapability(){
      if (wasEnabled) glEnable(cap);
      else glDisable(cap);
   }
};


struct GLScopedBlendFunc
{
    GLint srcRGB, dstRGB;
    GLint srcAlpha, dstAlpha;

    GLScopedBlendFunc(GLenum src, GLenum dst)
    {
        glGetIntegerv(GL_BLEND_SRC_RGB,   &srcRGB);
        glGetIntegerv(GL_BLEND_DST_RGB,   &dstRGB);
        glGetIntegerv(GL_BLEND_SRC_ALPHA, &srcAlpha);
        glGetIntegerv(GL_BLEND_DST_ALPHA, &dstAlpha);

        glBlendFunc(src, dst);
    }

    GLScopedBlendFunc(const GLScopedBlendFunc&) = delete;
    GLScopedBlendFunc& operator=(const GLScopedBlendFunc&) = delete;

    ~GLScopedBlendFunc()
    {
        glBlendFuncSeparate(
            srcRGB, dstRGB,
            srcAlpha, dstAlpha
        );
    }
};

struct GLScopedActiveTexture {
    GLint previous;

    GLScopedActiveTexture(GLenum newUnit) {
        glGetIntegerv(GL_ACTIVE_TEXTURE, &previous); // save current
        glActiveTexture(newUnit);                    // switch to new
    }

    GLScopedActiveTexture(const GLScopedActiveTexture&) = delete;
    GLScopedActiveTexture& operator=(const GLScopedActiveTexture&) = delete;

    // Destructor: restore previous active texture unit
    ~GLScopedActiveTexture() {
        glActiveTexture(previous);
    }

};
