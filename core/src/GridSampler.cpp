/*
* Copyright 2016 Nu-book Inc.
* Copyright 2016 ZXing authors
* Copyright 2020 Axel Waggershauser
*/
// SPDX-License-Identifier: Apache-2.0

#include "GridSampler.h"

#ifdef PRINT_DEBUG
#include "LogMatrix.h"
#include "BitMatrixIO.h"
#endif

namespace ZXing {

#ifdef PRINT_DEBUG
LogMatrix log;
#endif

DetectorResult SampleGrid(const BitMatrix& image, int width, int height, const PerspectiveTransform& mod2Pix)
{
	return SampleGrid(image, width, height, {ROI{0, width, 0, height, mod2Pix}});
}

DetectorResult SampleGrid(const BitMatrix& image, int width, int height, const ROIs& rois)
{
#ifdef PRINT_DEBUG
	LogMatrix log;
	static int i = 0;
	LogMatrixWriter lmw(log, image, 5, "grid" + std::to_string(i++) + ".pnm");
#endif
	if (width <= 0 || height <= 0)
		return {};

	BitMatrix res(width, height);
	for (auto&& [x0, x1, y0, y1, mod2Pix] : rois) {
		for (int y = y0; y < y1; ++y)
			for (int x = x0; x < x1; ++x) {
				auto p = mod2Pix(centered(PointI{x, y}));

				if (!image.isIn(p))
					return {};

#ifdef PRINT_DEBUG
				log(p, 3);
#endif
				if (image.get(p))
					res.set(x, y);
			}
	}

#ifdef PRINT_DEBUG
	printf("width: %d, height: %d\n", width, height);
//	printf("%s", ToString(res).c_str());
#endif

	auto projectCorner = [&](PointI p) {
		for (auto&& [x0, x1, y0, y1, mod2Pix] : rois)
			if (x0 <= p.x && p.x <= x1 && y0 <= p.y && p.y <= y1)
				return PointI(mod2Pix(PointF(p)) + PointF(0.5, 0.5));

		return PointI();
	};

	return {std::move(res),
			{projectCorner({0, 0}), projectCorner({width, 0}), projectCorner({width, height}), projectCorner({0, height})}};
	}

} // ZXing
