//
//  ContentView.swift
//  DylibProject
//
//  Created by MD Abir Hasan Zoha on 4/13/22.
//

import SwiftUI

#if targetEnvironment(simulator)
import walletintel
#else
import walletarm
#endif

struct ContentView: View {
    
    @State var walletPass: String = ""
    
    var body: some View {
        Text(walletPass)
            .padding()
            .task {
                await getPassword()
            }
    }
    
    func getPassword() async {
        let password = createNewWallet(nil, "password")
        
        let pass = String(cString: password!)
        
        walletPass = pass
        
        print("Password ", pass)
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
