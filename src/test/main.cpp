#include "Effect.h"
#include "Mesh.h"

#include "TestReporterStdout.h"
#include "TestRunner.h"
#include "UnitTest++.h"

#include <memory>

SUITE(PONG) {

    TEST(MeshQuadBuiltCorrectly) {
        auto mesh = buildQuadMesh(10, 10, buildOrthoEffect());
        CHECK_EQUAL(6, mesh->indexCount);

        std::uint32_t expectedIndices[] = {
            0, 1, 3,
            1, 2, 3
        };
        CHECK_ARRAY_EQUAL(expectedIndices, mesh->indices, 6);

        float expectedVertices[] = {
            5, 5, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            5, -5, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            -5, -5, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            -5,  5, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f
        };
        CHECK_ARRAY_EQUAL(expectedVertices, mesh->vertices, 32);
        CHECK_EQUAL(128, mesh->verticesTotalSize);
    }

}

int main() {
    return UnitTest::RunAllTests();
}
