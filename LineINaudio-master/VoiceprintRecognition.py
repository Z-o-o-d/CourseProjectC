import base64
import hashlib
import hmac
import json
from datetime import datetime
from time import mktime
from urllib.parse import urlencode
from wsgiref.handlers import format_date_time
import time
import requests

from pydub import AudioSegment


def convert_audio(input_path, output_path):
    # 打开音频文件
    audio = AudioSegment.from_file(input_path)

    # 设置采样率为16kHz，位长16bit，单声道
    audio = audio.set_frame_rate(16000)
    audio = audio.set_sample_width(2)  # 16bit = 2 bytes
    audio = audio.set_channels(1)  # 单声道

    # 导出为mp3格式
    audio.export(output_path, format="mp3", bitrate="128k")

def decode_base64_in_json(json_data):
    if isinstance(json_data, dict):
        for key, value in json_data.items():
            json_data[key] = decode_base64_in_json(value)
        return json_data
    elif isinstance(json_data, list):
        return [decode_base64_in_json(item) for item in json_data]
    elif isinstance(json_data, str):
        try:
            decoded_data = base64.b64decode(json_data).decode('utf-8')
            return decoded_data
        except Exception as e:
            return json_data
    else:
        return json_data

class Gen_req_url(object):
    """生成请求的url"""

    def sha256base64(self, data):
        sha256 = hashlib.sha256()
        sha256.update(data)
        digest = base64.b64encode(sha256.digest()).decode(encoding='utf-8')
        return digest

    def parse_url(self, requset_url):
        stidx = requset_url.index("://")
        host = requset_url[stidx + 3:]
        # self.schema = requset_url[:stidx + 3]
        edidx = host.index("/")
        if edidx <= 0:
            raise Exception("invalid request url:" + requset_url)
        self.path = host[edidx:]
        self.host = host[:edidx]

    # build websocket auth request url
    def assemble_ws_auth_url(self, requset_url, api_key, api_secret, method="GET"):
        self.parse_url(requset_url)
        now = datetime.now()
        date = format_date_time(mktime(now.timetuple()))
        # date = "Thu, 12 Dec 2019 01:57:27 GMT"
        signature_origin = "host: {}\ndate: {}\n{} {} HTTP/1.1".format(self.host, date, method, self.path)
        signature_sha = hmac.new(api_secret.encode('utf-8'), signature_origin.encode('utf-8'),
                                 digestmod=hashlib.sha256).digest()
        signature_sha = base64.b64encode(signature_sha).decode(encoding='utf-8')
        authorization_origin = "api_key=\"%s\", algorithm=\"%s\", headers=\"%s\", signature=\"%s\"" % (
            api_key, "hmac-sha256", "host date request-line", signature_sha)
        authorization = base64.b64encode(authorization_origin.encode('utf-8')).decode(encoding='utf-8')
        values = {
            "host": self.host,
            "date": date,
            "authorization": authorization
        }

        return requset_url + "?" + urlencode(values)


def gen_req_body(apiname, APPId, file_path=None):
    """
    生成请求的body
    :param apiname
    :param APPId: Appid
    :param file_name:  文件路径
    :return:
    """
    if apiname == 'createFeature':

        with open(file_path, "rb") as f:
            audioBytes = f.read()
        body = {
            "header": {
                "app_id": APPId,
                "status": 3
            },
            "parameter": {
                "s782b4996": {
                    "func": "createFeature",
                    "groupId": "iFLYTEK_examples_groupId",
                    "featureId": "VeriMake",
                    "featureInfo": "VeriMake",
                    "createFeatureRes": {
                        "encoding": "utf8",
                        "compress": "raw",
                        "format": "json"
                    }
                }
            },
            "payload": {
                "resource": {
                    "encoding": "lame",
                    "sample_rate": 16000,
                    "channels": 1,
                    "bit_depth": 16,
                    "status": 3,
                    "audio": str(base64.b64encode(audioBytes), 'UTF-8')
                }
            }
        }
    elif apiname == 'createGroup':

        body = {
            "header": {
                "app_id": APPId,
                "status": 3
            },
            "parameter": {
                "s782b4996": {
                    "func": "createGroup",
                    "groupId": "iFLYTEK_examples_groupId",
                    "groupName": "iFLYTEK_examples_groupName",
                    "groupInfo": "iFLYTEK_examples_groupInfo",
                    "createGroupRes": {
                        "encoding": "utf8",
                        "compress": "raw",
                        "format": "json"
                    }
                }
            }
        }
    elif apiname == 'deleteFeature':

        body = {
            "header": {
                "app_id": APPId,
                "status": 3

            },
            "parameter": {
                "s782b4996": {
                    "func": "deleteFeature",
                    "groupId": "iFLYTEK_examples_groupId",
                    "featureId": "iFLYTEK_examples_featureId",
                    "deleteFeatureRes": {
                        "encoding": "utf8",
                        "compress": "raw",
                        "format": "json"
                    }
                }
            }
        }
    elif apiname == 'queryFeatureList':

        body = {
            "header": {
                "app_id": APPId,
                "status": 3
            },
            "parameter": {
                "s782b4996": {
                    "func": "queryFeatureList",
                    "groupId": "iFLYTEK_examples_groupId",
                    "queryFeatureListRes": {
                        "encoding": "utf8",
                        "compress": "raw",
                        "format": "json"
                    }
                }
            }
        }
    elif apiname == 'searchFea':

        with open(file_path, "rb") as f:
            audioBytes = f.read()
        body = {
            "header": {
                "app_id": APPId,
                "status": 3
            },
            "parameter": {
                "s782b4996": {
                    "func": "searchFea",
                    "groupId": "iFLYTEK_examples_groupId",
                    "topK": 1,
                    "searchFeaRes": {
                        "encoding": "utf8",
                        "compress": "raw",
                        "format": "json"
                    }
                }
            },
            "payload": {
                "resource": {
                    "encoding": "lame",
                    "sample_rate": 16000,
                    "channels": 1,
                    "bit_depth": 16,
                    "status": 3,
                    "audio": str(base64.b64encode(audioBytes), 'UTF-8')
                }
            }
        }
    elif apiname == 'searchScoreFea':

        with open(file_path, "rb") as f:
            audioBytes = f.read()
        body = {
            "header": {
                "app_id": APPId,
                "status": 3
            },
            "parameter": {
                "s782b4996": {
                    "func": "searchScoreFea",
                    "groupId": "iFLYTEK_examples_groupId",
                    "dstFeatureId": "iFLYTEK_examples_featureId",
                    "searchScoreFeaRes": {
                        "encoding": "utf8",
                        "compress": "raw",
                        "format": "json"
                    }
                }
            },
            "payload": {
                "resource": {
                    "encoding": "lame",
                    "sample_rate": 16000,
                    "channels": 1,
                    "bit_depth": 16,
                    "status": 3,
                    "audio": str(base64.b64encode(audioBytes), 'UTF-8')
                }
            }
        }
    elif apiname == 'updateFeature':

        with open(file_path, "rb") as f:
            audioBytes = f.read()
        body = {
            "header": {
                "app_id": APPId,
                "status": 3
            },
            "parameter": {
                "s782b4996": {
                    "func": "updateFeature",
                    "groupId": "iFLYTEK_examples_groupId",
                    "featureId": "iFLYTEK_examples_featureId",
                    "featureInfo": "iFLYTEK_examples_featureInfo_update",
                    "updateFeatureRes": {
                        "encoding": "utf8",
                        "compress": "raw",
                        "format": "json"
                    }
                }
            },
            "payload": {
                "resource": {
                    "encoding": "lame",
                    "sample_rate": 16000,
                    "channels": 1,
                    "bit_depth": 16,
                    "status": 3,
                    "audio": str(base64.b64encode(audioBytes), 'UTF-8')
                }
            }
        }
    elif apiname == 'deleteGroup':
        body = {
            "header": {
                "app_id": APPId,
                "status": 3
            },
            "parameter": {
                "s782b4996": {
                    "func": "deleteGroup",
                    "groupId": "iFLYTEK_examples_groupId",
                    "deleteGroupRes": {
                        "encoding": "utf8",
                        "compress": "raw",
                        "format": "json"
                    }
                }
            }
        }
    else:
        raise Exception(
            "输入的apiname不在[createFeature, createGroup, deleteFeature, queryFeatureList, searchFea, searchScoreFea,updateFeature]内，请检查")
    return body


def req_url(api_name, APPId, APIKey, APISecret, file_path=None, output_file="VoiceprintRecognition.txt"):
    gen_req_url = Gen_req_url()
    body = gen_req_body(apiname=api_name, APPId=APPId, file_path=file_path)
    request_url = gen_req_url.assemble_ws_auth_url(requset_url='https://api.xf-yun.com/v1/private/s782b4996', method="POST", api_key=APIKey, api_secret=APISecret)

    headers = {'content-type': "application/json", 'host': 'api.xf-yun.com', 'appid': '$APPID'}
    response = requests.post(request_url, data=json.dumps(body), headers=headers)
    tempResult = json.loads(response.content.decode('utf-8'))
    print(tempResult)
    tempResult = decode_base64_in_json(tempResult)
    print(tempResult)

    # 保存结果到txt文件
    with open(output_file, "w") as f:
        f.write(json.dumps(tempResult, indent=4))
    print("结果已保存到", output_file)


if __name__ == '__main__':
    while True:
        input_audio_path = "recording.mp3"
        output_audio_path = "output_converted.mp3"
        convert_audio(input_audio_path, output_audio_path)

        file_path = 'output_converted.mp3'
        req_url("****************")
        time.sleep(0.5)