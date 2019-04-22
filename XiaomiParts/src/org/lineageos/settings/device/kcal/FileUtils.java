/*
 * Copyright (C) 2018 The Xiaomi-SDM660 Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License
 */

package org.lineageos.settings.device.kcal;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

class FileUtils {

    private static boolean fileWritable(String filename) {
        return fileExists(filename) && new File(filename).canWrite();
    }

    private static boolean fileExists(String filename) {
        if (filename == null) {
            return false;
        }
        return new File(filename).exists();
    }

    void setValue(String path, Boolean value) {
        if (fileWritable(path)) {
            if (path == null) {
                return;
            }
            try {
                FileOutputStream fos = new FileOutputStream(new File(path));
                fos.write((value ? "1" : "0").getBytes());
                fos.flush();
                fos.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    void setValue(String path, int value) {
        if (fileWritable(path)) {
            if (path == null) {
                return;
            }
            try {
                FileOutputStream fos = new FileOutputStream(new File(path));
                fos.write(Integer.toString(value).getBytes());
                fos.flush();
                fos.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    void setValue(String path, String value) {
        if (fileWritable(path)) {
            if (path == null) {
                return;
            }
            try {
                FileOutputStream fos = new FileOutputStream(new File(path));
                fos.write(value.getBytes());
                fos.flush();
                fos.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}
