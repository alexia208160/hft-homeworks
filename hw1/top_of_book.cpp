#include <cstdio>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <numeric>
#include <iostream>

struct Metrics { double mid, spread, micro, obi; };

Metrics compute(double bp, double bs, double ap, double as);
void report(Metrics n);

int main() {

    std::ifstream file("snapshots.csv");
    std::string line;
    std::getline(file, line);
    std::vector<double> obs;
    std::vector<double> mids;

    double prev_mid = -1, prev_obi;
    double bid_px, bid_sz, ask_px, ask_sz;

    while (std::getline(file, line)) {
       
        std::vector<double> row;
        std::stringstream ss(line);
        std::string cell;
        std::string timestamp;
        std::string ticker;

        std::getline(ss, timestamp, ','); //timestamp
        std::getline(ss, ticker, ','); //ticker

        while (std::getline(ss, cell, ',')) {
            row.push_back(std::stod(cell));
        }

        bid_px = row[0];
        bid_sz = row[1];
        ask_px = row[2];
        ask_sz = row[3];

        if (ask_px <= bid_px) {
            printf("Crossed/Locked market: Bid: %.4f >= Ask: %.4f\n", bid_px, ask_px);
        }

        if(bid_sz == ask_sz)
            printf("Balanced %s:", timestamp.c_str());
        else{
            if(bid_sz > ask_sz)
                printf("Bid heavy %s:", timestamp.c_str());
            else
                printf("Ask heavy %s:", timestamp.c_str());
        }
        Metrics n = compute(bid_px, bid_sz, ask_px, ask_sz);

        report(n);

        if (prev_mid >= 0) {
            double d_mid = n.mid - prev_mid;
            double d_obi = n.obi - prev_obi;
            mids.push_back(d_mid);
            obs.push_back(d_obi);

            printf("  d_mid=%+.4f d_obi=%+.4f", d_mid, d_obi);
            if (d_obi > 0)
                printf("  -> OBI rising: buy-side pressure building, mid likely to tick up next\n");
            else if (d_obi < 0)
                printf("  -> OBI falling: sell-side pressure building, mid likely to tick down next\n");
            else
                printf("  -> OBI unchanged\n");
        }
        printf("\n");

        prev_mid = n.mid;
        prev_obi = n.obi;

    }

    printf("\nmid/OBI evolution\n");
    for (size_t i = 0; i < mids.size(); ++i) {
        printf("step %zu: d_mid=%+.4f d_obi=%+.4f", i + 1, mids[i], obs[i]);
        if (obs[i] > 0)
            printf("\nOBI rising: buy-side pressure, mid likely to tick up next\n");
        else if (obs[i] < 0)
            printf("\nOBI falling: sell-side pressure, mid likely to tick down next\n");
        else
            printf("\nOBI unchanged\n");
    }

    file.close();
    return 0;
}

Metrics compute(const double bp, const double bs, const double ap, const double as){

    return Metrics{(double)(bp + ap)/2, //mid
        (double) ap-bp, //spread
        (double) (ap*bs + bp * as)/(bs + as), //micro
        (double)(bs-as)/(bs + as) }; //obi

}

void report(const Metrics n){
    printf("\nmid=%.4f spread=%.4f micro=%.4f obi=%.4f\n",
           n.mid, n.spread, n.micro, n.obi);
}
