![Logo of the project](https://www.zemana.com/images/logo/new-zmn-text-blue.png)

# PE feature extractor
> PE file format extractor for AI projects

A simple, easy-to-use script to extract useful features to evaluate in deep learning or machine learning projects

## Getting started

It is tested in Python 3.6 version. The other versions of python have not been tested yet.

First, define your properties and feature requests.

```code
features = {'size': 8, 
            'entry_point':512, 
            'code_entropy':8,
            'entropies': 50,
            'header':1024, 
            'resources':1024,
            'security':1024,
            'imports':1024, 
            'exports':1024}
dirs = ['train/malware', 'train/clean', 
        'test/malware', 'test/clean']
```

then extract your features into temprorary directory

```code
extract_features(dirs, output='out_feature', **features)
```

you can load those features into memory by using that directory and the features you defined previously.

```code
x_train, y_train, x_test, y_test = load_features(dirs, output='out_feature', **features)
```
NOTE: You can decrease featre sizes in the load operation. Loader will cut the unnecessary bytes for you.


## Contributing

The battle between the bad and the good. Send the bugs, feature requests and code submits. All are welcome and thank you for your contribution.

Believe in us as much as we believe in you. We need your help to make it better.

## Licensing

"The code in this project is licensed under MIT license."
