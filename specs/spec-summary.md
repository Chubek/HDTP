# Hyper Data Transfer Protocol - Spec Summary

## Philosopy

HDTP is either a simplified, or a more convoluted reinterpretation of HTTP, depending on how you look at it.

HTTP was designated for open access to documents. It was stacked on top of TCP and it's only secure coupled with SSL/TSL. It's designed to transfer text documents and binary data and they can be anything. 

HDTP is designed for sharing of formatted data FILES, that is, files indexed in a folder being watched by the background server daemon. Including metadata with these files is necessary. However, the metadata shall rest in a separate file, with the same name as the formatted data file. The metadata is simply formatted as a colon-seperated, multiline document. For example:

`~/.hdtp/indices/my_data.json`

```json
"key": {
    "A": 1,
    "B": "S",
    "C": [1, 2, 3]
}

```

`~/.hdtp/indices/my_index/metadata/my_data.json.hdmd`

```
v1

Stock:
    Name: my_data;
    Mime-Type: Text/JSON;
    Date-Created: 24-3-2022;
    ...

Custom:
    My-Custom-Message: 'feel_well';
    ...
```

The extension for metadata files should be `hdmd`. The file contains a mandatory `Stock` section and a custom `Custom` section, **Custom and Stck section are separated by two newlines**. Using two newlines, unless in multi-line strings, which are encapsulated by thre single or double quotes; `'''` and `"""` respectively, is NOT permitted. Single line strings can be encapsulated with a sole single or double quote. Each line should be terminated with a semicolon and a newline. No return carriage (`\r`) necessary.

The spec version should be specified on top of the file. **Spec version must be separated by two newlines from rest of the file**. As you may have noticed, HDMDv1 is newline-aware. To be informed about the entirety of v1 HDMD specs, please look inside the `/metadata-specs` folder.

The medadata files can be placed both inside the `metadata` folder, which is a subfolder of the index, and the same folder as the data files.

My current implementation of HDTP uses QUIC, and not TCP. It uses MsQuic in C to transport the application layer which is HDTP. You are free to make a TCP version. Just credit me please. Thanks.

## The Client

The client may request to list the index with `hdtp-index-down`. The metadata for each indexed file can be displayed with `info`. Finally, `get` will download the file and the metadata file and store them in specified folder.


```
$ hdtp-index-down hdtp://8.8.8.8/my_index

 ### HDTP v1 Server     ###
 ### Admin: Joy Montana ###
+ joy_json_data.json
+ joy_yaml_data.yaml
+ joy_xml_data.xml

info joy_json_data.json

+Displaying info for `joy_json_data.json`:

Stock:
    Name: Joy's Wedding Guests;
    Mime-Type: Text/YAML;
    Date-Created: 12-3-2011;
    ...

Custom:
    Wedding-Venue: Johnston Saint-Paris;
    Wedding-Date: 12-5-2012;
    ...

- /Info

get joy_json_data.json .

exit
```

This will display all the files inside the index, show the info for one of them, and download one.

```
get file1 file2 ./path
```

Downloads multiple files named `file1` and `file2` in the index. 

```
get * ./path
```

Downloads the entire index.

```
get --inplacemd file1 ~/path
```

Will download `file1` and places the metadata file inside the same folder.

YOu cannot download a file directly. You must gain access to the index first. The index may be protected by a password. 'Hyperlinks` are not a thing with this protocol.

## The Server

To index a folder that you have prepared, you must use command `hdtp-index-up /mnt/folder`. By default it will symbolically link this folder to `~/.hdtp/indices`. You can pass `-cpy` to copy the folder. you can likewise pass `-sym` to force symbolic linking. 

You may also manually copy or symlink the folder to the `~/hdtp/indices` folder. Just make sure you restart the service after that.

The service is shipped as a `systemd` service. It runs `hdtp-index-daemon`. But `hdtp-index-up` is the binary that handles the exposure.

The `.config` file inside the index folder (for example `~/.hdtp/indices/my_index/.config`) allows you to set certain configurations such as password access, domain name, port (hdtp accepts port per index), etc. To see the full specs for the config file see `/config-steps/v1.md`. 

## Suppoted MIME-Types and Verification

Only MIME-Types that are textual in nature are accepted. One of the stock metadata is the encoding which must be set. 

You can usse your own data format as well. But mind you that unknown data formats cannot be verified. When you are inside the client REPL you can ask to verify a certain file for example `verify my_yaml.yaml`. Then the client will send a verification request to the server, and server will verify the file by looking for certain markers and characters and snytax associated with that format. And if so, it will then send back a `Verified` message. If not it will send back a `Unverified` message.

## How is the Data Transfered?

The selected files will be serially assembled. First the metadata, then the files themselves. The application layer for requesting two JSON file looks like this:

```
$Metadata:
- my_json_1.json.hdmd:
+++
<metadata for 1>
+++
        #this is \n\r
- my_json_two.json.hdmd:
+++
<metadata for 2>
+++
        #this is \n\r
        #this is \n\r
$Data:
- my_json_1.json:
+++
<data>
+++
        #this is \n\r
- my_json_two.json:
+++
<data>
+++
        #last line is just a NULL char (\x00)
```

After4 the client receives them, it will de-assemble them and save them to necessary paths.

When requesting for info, this is sent:

```
$REVEAL: my_json.json #NULL-terminated
```

and ther the contents of the file are sent back intact.

When requesting one or multiple files to download this is the request:

```
$DOWN: my_json.json;
$DOWN: my_yaml.yaml;
            #null after newline
```

And for all files:

```
$DOWN_ALL #null_terminated
```

The password is sent as a hash.

```
$PWD: <hash> #null_terminaed
```

Either one of these are sent:

```
$ALLOWED #correct pass #either
$DENIED #incorrect pass #or
```

Or after 3 tries, it will block the client for specified time, if configured (which is by default to 1 hour). If so server will send back:

```
$BLOCKED #ouch!
```

## QUIC Outlook