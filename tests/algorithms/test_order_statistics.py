import pytest

# verify with actual value
import statistics
import math

import pydp as dp
from conftest import expect_near, percentile

kDataSize = 10000
data_floats = [200.0 * i / kDataSize for i in range(kDataSize)]
data_ints = [int(200 * i / kDataSize) for i in range(kDataSize)]

# NOTE: One needs to use https://pypi.org/project/pytest-lazy-fixture/ to have fixtues in parameters.
@pytest.mark.parametrize("dtype, data", [("int", data_ints), ("float", data_floats)])
def test_min(data, dtype):

    minn = dp.algorithms.laplacian.Min(
        dtype=dtype, epsilon=1.0, lower_bound=0, upper_bound=2048
    )

    assert expect_near(min(data), minn.result(data), 10)
    assert expect_near(0, minn.result(data), 10)


@pytest.mark.parametrize("dtype, data", [("int", data_ints), ("float", data_floats)])
def test_max(data, dtype):
    maxx = dp.algorithms.laplacian.Max(
        dtype=dtype, epsilon=1.0, lower_bound=0, upper_bound=2048
    )
    assert 190 < maxx.result(data) < 210

    assert expect_near(max(data), maxx.result(data), 10)


@pytest.mark.parametrize("dtype, data", [("int", data_ints), ("float", data_floats)])
def test_median(data, dtype):

    median = dp.algorithms.laplacian.Median(
        dtype=dtype, epsilon=1.0, lower_bound=0, upper_bound=2048
    )

    assert expect_near(statistics.median(data), median.result(data), 20)


@pytest.mark.parametrize("dtype, data", [("int", data_ints), ("float", data_floats)])
def test_percentile(data, dtype):
    maxx = dp.algorithms.laplacian.Percentile(
        dtype=dtype, epsilon=1.0, lower_bound=0, upper_bound=2048
    )
    maxx.percentile = 0.45

    actual_percentile = int(percentile(data, 0.45))

    assert maxx.percentile == 0.45
    # TODO: Unsure if this is a valid test.
    # assert expect_near(actual_percentile, maxx.result(data) * 100, 10)


@pytest.mark.parametrize("dtype, data", [("int", data_ints), ("float", data_floats)])
def test_max_datatypes(data, dtype):
    ma1 = dp.algorithms.laplacian.Max(dtype=dtype, epsilon=1.0)
    ma2 = dp.algorithms.laplacian.Max(dtype=dtype, epsilon=1.0, lower_bound=0, upper_bound=2048)
    assert isinstance(ma1, dp.algorithms.laplacian.Max)
    assert isinstance(ma2, dp.algorithms.laplacian.Max)

    res = ma2.result(data)
    assert isinstance(res, type(data[0]))


# def test_min_datatypes(data):
#     mi1 = dp.Min(1.0)
#     mi2 = dp.Min(1.0, 0, 2048)
#     assert isinstance(mi1, dp.Min)
#     assert isinstance(mi2, dp.Min)

#     res = mi2.result(data, 1.0)
#     assert isinstance(res, float)


# def test_median_datatypes(data):
#     me1 = dp.Median(1.0)
#     me2 = dp.Median(1.0, 0, 2048)
#     assert isinstance(me1, dp.Median)
#     assert isinstance(me2, dp.Median)

#     res = me2.result(data, 1.0)
#     assert isinstance(res, float)


# def test_percentile_datatypes(data):
#     pe1 = dp.Percentile(1.0)
#     pe2 = dp.Percentile(1.0, 0, 2048)
#     assert isinstance(pe1, dp.Percentile)
#     assert isinstance(pe2, dp.Percentile)

#     res = pe2.result(data, 1.0)
#     assert isinstance(res, float)
#     # resg = pe2.getPercentile()
#     # assert isinstance(resg, float)
#     # ress = pe2.setPercentile(0.5)
#     # assert isinstance(ress, None)


# TODO Yet some more tests
