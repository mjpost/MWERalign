# MWERalign

## What's new

- Modernized usage
- Simplified codebase

## Citation

If you use this package, please cite the following two papers. We suggest a sentence
similar to the following: "To align the text, we used the mweralign package \citep{post-huang-2025-effects},
which implements a variant of the AS-WER algorithm \citep{matusov-etal-2005-evaluating}.

> @inproceedings{matusov2005evaluating,
>   title={Evaluating machine translation output with automatic sentence segmentation},
>   author={Matusov, Evgeny and Leusch, Gregor and Bender, Oliver and Ney, Hermann},
>   booktitle={IWSLT 2005},
>   pages={138--144},
>   year={2005}
> }

> @inproceedings{post-huang-2025-effects,
>   title={Effects of automatic alignment on speech translation metric},
>   author={Post, Matt and Hoang, Hieu},
>   booktitle={Proceedings of IWSLT 2025},
>   year={2025}
> }


## License

This project contains code under multiple licenses:

- **Original C++ alignment code**: GNU General Public License v3 (GPL-3.0)
- **Python bindings and wrapper code**: Apache License 2.0
- **Build scripts and documentation**: Apache License 2.0

**The project as a whole is distributed under GPL-3.0** due to the inclusion of GPL-licensed components.

### What this means for users:
- You can use this library in GPL-compatible projects
- If you distribute software that includes this library, your software must be GPL-compatible
- The Python wrapper code (separate from the C++ core) is available under Apache License 2.0

## Attribution
This software includes original GPL-licensed C++ code for alignment algorithms.
Python bindings and packaging by Matt Post (Apache License 2.0).
