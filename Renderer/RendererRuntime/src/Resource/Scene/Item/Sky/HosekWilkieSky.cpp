/*********************************************************\
 * Copyright (c) 2012-2017 The Unrimp Team
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
\*********************************************************/


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/PrecompiledHeader.h"
#include "RendererRuntime/Resource/Scene/Item/Sky/HosekWilkieSky.h"


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global variables                                      ]
		//[-------------------------------------------------------]
		#include "RendererRuntime/Resource/Scene/Item/Sky/ArHosekSkyModelData_RGB.h"


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		double evaluateSpline(const double* spline, size_t stride, double value)
		{
			return
				1 *  std::pow(1 - value, 5) *                      spline[0 * stride] +
				5 *  std::pow(1 - value, 4) * std::pow(value, 1) * spline[1 * stride] +
				10 * std::pow(1 - value, 3) * std::pow(value, 2) * spline[2 * stride] +
				10 * std::pow(1 - value, 2) * std::pow(value, 3) * spline[3 * stride] +
				5 *  std::pow(1 - value, 1) * std::pow(value, 4) * spline[4 * stride] +
				1 *                           std::pow(value, 5) * spline[5 * stride];
		}

		float evaluate(const double* dataset, size_t stride, float turbidity, float albedo, float sunTheta)
		{
			// Splines are functions of elevation^1/3
			const double elevationK = std::pow(std::max<float>(0.0f, 1.0f - sunTheta / (glm::pi<float>() / 2.0f)), 1.0f / 3.0f);

			// Table has values for turbidity 1..10
			const int turbidity0 = glm::clamp<int>(static_cast<int>(turbidity), 1, 10);
			const int turbidity1 = std::min(turbidity0 + 1, 10);
			const float turbidityK = glm::clamp(turbidity - turbidity0, 0.0f, 1.0f);

			const double * datasetA0 = dataset;
			const double * datasetA1 = dataset + stride * 6 * 10;

			const double a0t0 = evaluateSpline(datasetA0 + stride * 6 * (turbidity0 - 1), stride, elevationK);
			const double a1t0 = evaluateSpline(datasetA1 + stride * 6 * (turbidity0 - 1), stride, elevationK);
			const double a0t1 = evaluateSpline(datasetA0 + stride * 6 * (turbidity1 - 1), stride, elevationK);
			const double a1t1 = evaluateSpline(datasetA1 + stride * 6 * (turbidity1 - 1), stride, elevationK);

			return static_cast<float>(a0t0 * (1 - albedo) * (1 - turbidityK) + a1t0 * albedo * (1 - turbidityK) + a0t1 * (1 - albedo) * turbidityK + a1t1 * albedo * turbidityK);
		}

		glm::vec3 hosekWilkie(float cos_theta, float gamma, float cos_gamma, const glm::vec3& A, const glm::vec3& B, const glm::vec3& C, const glm::vec3& D, const glm::vec3& E, const glm::vec3& F, const glm::vec3& G, const glm::vec3& H, const glm::vec3& I)
		{
			const glm::vec3 chi = (1.0f + cos_gamma * cos_gamma) / glm::pow(1.0f + H * H - 2.0f * cos_gamma * H, glm::vec3(1.5f));
			return (1.0f + A * glm::exp(B / (cos_theta + 0.01f))) * (C + D * glm::exp(E * gamma) + F * (cos_gamma * cos_gamma) + G * chi + I * static_cast<float>(std::sqrt(std::max(0.0f, cos_theta))));
		}

		/*
		TOOD(co) Can this be used for procedural sun color? Other solution at "Simple Solar Radiance Calculation" - https://www.gamedev.net/topic/671214-simple-solar-radiance-calculation/ 
		float perez(float theta, float gamma, float A, float B, float C, float D, float E)
		{
			return static_cast<float>(1.0f + A * glm::exp(B / (std::cos(theta) + 0.01))) * (1.0f + C * glm::exp(D * gamma) + E * std::cos(gamma) * std::cos(gamma));
		}

		float zenith_luminance(float sunTheta, float turbidity)
		{
			const float chi = (4.0f / 9.0f - turbidity / 120) * (glm::pi<float>() - 2 * sunTheta);
			return static_cast<float>((4.0453 * turbidity - 4.9710) * std::tan(chi) - 0.2155 * turbidity + 2.4192);
		}

		float zenith_chromacity(const glm::vec4& c0, const glm::vec4& c1, const glm::vec4& c2, float sunTheta, float turbidity)
		{
			const glm::vec4 thetav = glm::vec4(sunTheta * sunTheta * sunTheta, sunTheta * sunTheta, sunTheta, 1);
			return dot(glm::vec3(turbidity * turbidity, turbidity, 1), glm::vec3(dot(thetav, c0), dot(thetav, c1), dot(thetav, c2)));
		}
		*/

		RendererRuntime::HosekWilkieSky::Coefficients compute(const glm::vec3& worldSpaceSunDirection, float turbidity, float albedo, float normalizedSunY)
		{
			glm::vec3 A, B, C, D, E, F, G, H, I;
			glm::vec3 Z;

			const float sunTheta = std::acos(glm::clamp(worldSpaceSunDirection.y, 0.0f, 1.0f));

			for (int i = 0; i < 3; ++i)
			{
				A[i] = evaluate(datasetsRGB[i] + 0, 9, turbidity, albedo, sunTheta);
				B[i] = evaluate(datasetsRGB[i] + 1, 9, turbidity, albedo, sunTheta);
				C[i] = evaluate(datasetsRGB[i] + 2, 9, turbidity, albedo, sunTheta);
				D[i] = evaluate(datasetsRGB[i] + 3, 9, turbidity, albedo, sunTheta);
				E[i] = evaluate(datasetsRGB[i] + 4, 9, turbidity, albedo, sunTheta);
				F[i] = evaluate(datasetsRGB[i] + 5, 9, turbidity, albedo, sunTheta);
				G[i] = evaluate(datasetsRGB[i] + 6, 9, turbidity, albedo, sunTheta);

				// Swapped in the dataset
				H[i] = evaluate(datasetsRGB[i] + 8, 9, turbidity, albedo, sunTheta);
				I[i] = evaluate(datasetsRGB[i] + 7, 9, turbidity, albedo, sunTheta);

				Z[i] = evaluate(datasetsRGBRad[i], 1, turbidity, albedo, sunTheta);
			}

			if (normalizedSunY)
			{
				const glm::vec3 S = hosekWilkie(std::cos(sunTheta), 0, 1.0f, A, B, C, D, E, F, G, H, I) * Z;
				Z /= glm::dot(S, glm::vec3(0.2126f, 0.7152f, 0.0722f));
				Z *= normalizedSunY;
			}

			return {A, B, C, D, E, F, G, H, I, Z};
		}


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
	} // detail
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	void HosekWilkieSky::recalculate(const glm::vec3& worldSpaceSunDirection, float turbidity, float albedo, float normalizedSunY)
	{
		if (mWorldSpaceSunDirection != worldSpaceSunDirection || mTurbidity != turbidity || mAlbedo != albedo || mNormalizedSunY != normalizedSunY)
		{
			mWorldSpaceSunDirection = worldSpaceSunDirection;
			mTurbidity = turbidity;
			mAlbedo = albedo;
			mNormalizedSunY = normalizedSunY;
			mCoefficients = ::detail::compute(mWorldSpaceSunDirection, mTurbidity, mAlbedo, mNormalizedSunY);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
