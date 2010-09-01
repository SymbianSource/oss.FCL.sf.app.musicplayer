rem
rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description:  Delete old sis file, creating new sis package and sign it.
rem

makesis -v mpxmusicplayer.pkg
signsis mpxmusicplayer.sis mpxmusicplayer_0x101FFC62_v15.0.0911_SA_S60.50_Eng.sis Nokia_RnDCert_02.der Nokia_RnDCert_02.key