# %%
# !%load_ext autoreload
# !%autoreload 2
import pydp

print(pydp.__file__)
import random

# %%
print(pydp.LaplaceDistribution(1.0))

# %%
pdl = pydp.LaplaceDistribution.
pdl
# %%
pdl.sample(scale=10.0)
help(pdl.sample)
# %%
