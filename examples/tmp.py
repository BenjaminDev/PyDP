import pydp as dp

c=dp.CountInt(1.0)
c.add_entries([1,2,3,4])
c.partial_result(0.5)
try:
    c.partial_result(0.7)
except:
    print("caught")