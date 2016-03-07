.. _hdfwriter-faq:

FAQ
===

Can I use hdfwriter to write from files with only Q frames, or would I have to create P frames on my own with those objects that I want to have in the hdf5 file?
-----------------------------------------------------------------------------------------------------------------------------------------------------------------

The later. Put an I3NullSplitter in front of I3HDFWriter.
